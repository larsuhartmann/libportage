/*
 * Copyright (c) 2008, Lars Hartmann
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *      
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *
 *    3. Neither the name of the Original Author, nor the names of its
 *       contributors may be used to endorse or promote products derived from
 *       this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "liblpxpak.h"

#include <sys/stat.h>
#include <arpa/inet.h>

#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>


#define __LPXPAK_STOP_OFFSET    4
#define __LPXPAK_OFFSET         8
#define __LPXPAK_INTRO_LEN      8
#define __LPXPAK_INTRO          "XPAKPACK"
#define __LPXPAK_OUTRO_LEN      8
#define __LPXPAK_OUTRO          "XPAKSTOP"
#define __LPXPAK_STOP_LEN       4
#define __LPXPAK_STOP           "STOP"

typedef uint32_t __lpxpak_int_t;

typedef struct __lpxpak_index{
     char *name;
     size_t offset;
     size_t len;
     struct __lpxpak_index *next;
} __lpxpak_index_t;

static __lpxpak_index_t *
__lpxpak_parse_index(const void *data, size_t len);

static lpxpak_t *
__lpxpak_parse_data(const void *data, __lpxpak_index_t *index);

static void
__lpxpak_destroy_index(__lpxpak_index_t *index);

static void
__lpxpak_init_index(__lpxpak_index_t *index);

static void
__lpxpak_init(lpxpak_t *xpak);

/* 
 * lpxpak_parse_data: Reads the xpak data out of a xpak binary blob.
 *
 * Gets the actual xpak-blob (see doc/xpak.txt) and returns an pointer to an
 * lpxpak object with its data. If an error occurs, NULL is returned and errno
 * is set to indicate the error.
 *
 * Errors:
 *         EINVAL The file either is no valid gentoo binary package or has an
 *                invalid xpak.
 *                
 *         EBUSY The xpak could not be fully read in
 *
 *         The lpxpak_parse_data() function may also fail and set errno for
 *         any of the errors specified for the routine malloc(3).
 */
lpxpak_t *
lpxpak_parse_data(const void *data, size_t len)
{
     __lpxpak_int_t count = 0;
     __lpxpak_int_t index_len, data_len;
     const void *index_data = NULL;
     const void *data_data = NULL;
     __lpxpak_index_t *index = NULL;
     __lpxpak_index_t *ti;
     size_t tl = 0;
     lpxpak_t *xpak = NULL;
     
     /* check if the first __LPXPAK_INTRO_LEN bytes of the xpak data read
      * __LPXPAK_INTRO and the last __LPXPAK_OUTRO_LEN bytes of the xpak read
      * __LPXPAK_OUTRO to make sure we have a valid xpak, if not, set errno
      * and return. Otherwise increase count which we will be using as an seek
      * counter */
     if ((memcmp(data, __LPXPAK_INTRO, __LPXPAK_INTRO_LEN) != 0) ||
         (memcmp(data+len-__LPXPAK_OUTRO_LEN, __LPXPAK_OUTRO,
                 __LPXPAK_OUTRO_LEN != 0))) {
          errno = EINVAL;
          return NULL;
     }
     count += __LPXPAK_INTRO_LEN;

     memcpy(&index_len, data+count, sizeof(__lpxpak_int_t));
     count+=sizeof(__lpxpak_int_t);
     index_len = ntohl(index_len);
     memcpy(&data_len, data+count, sizeof(__lpxpak_int_t));
     count += sizeof(__lpxpak_int_t);
     data_len = ntohl(data_len);

     /* check if the sum of count, index_len, data_len and __LPXPAK_OUTRO_LEN
      * is equal to len to make sure the len values are correct */
     if ( count+index_len+data_len+__LPXPAK_OUTRO_LEN != len ) {
          errno = EINVAL;
          return NULL;
     }

     index_data = data+count;
     data_data = data+count+index_len;

     /* get index */
     index = __lpxpak_parse_index(index_data, (size_t)index_len);
     /* check if the sum of all len entries of all data elements is equal to
      * data_len to make sure the len values are correct, if not, clean up the
      * heap, set errno and return.  */
     ti = index;
     while (ti!=NULL) {
          tl += ti->len;
          ti = ti->next;
     }
     if (tl != data_len) {
          errno = EINVAL;
          return NULL;
     }
     ti = NULL;

     /* get xpak-data  */
     xpak = __lpxpak_parse_data(data_data, index);

     /* free up index */
     __lpxpak_destroy_index(index);

     return xpak;
}

/* 
 * lpxpak_parse_fd: Reads the xpak data out of a file-descriptor which points
 *                  to an Gentoo binary package.
 *
 * Gets an file-descriptor (fd) for a Gentoo binary package and returns an
 * pointer to an lpxpak object with the xpak data. If an error occurs, NULL is
 * returned and errno is set to indicate the error.
 *
 * Errors:
 *         EINVAL The file either is no valid gentoo binary package or has an
 *                invalid xpak.
 *                
 *         EBUSY The xpak could not be fully read in
 *
 *         The lpxpak_parse_fd() function may also fail and set errno for any
 *         of the errors specified for the routine malloc(3).
 *
 *         The lpxpak_parse_fd() function may also fail and set errno for any
 *         of the errors specified for the routine lseek(2).
 *
 *         The lpxpak_parse_fd() function may also fail and set errno for any
 *         of the errors specified for the routine read(2).
 *
 *         The lpxpak_parse_fd() function may also fail and set errno for any
 *         of the errors specified for the routine fstat(2).
 */
lpxpak_t *
lpxpak_parse_fd(int fd)
{
     struct stat xpakstat;
     void *xpakdata = NULL;
     void *tmp = NULL;
     __lpxpak_int_t *xpakoffset = NULL;
     lpxpak_t *xpak = NULL;
     ssize_t rs;

     /* check if the given fd belongs to a file */
     if ( fstat(fd, &xpakstat) == -1 )
          return NULL;
     if (! S_ISREG(xpakstat.st_mode) ) {
          errno = EINVAL;
          return NULL;
     }

     /* seek to the start of the xpak offset */
     if ( lseek(fd, (__LPXPAK_STOP_OFFSET+sizeof(__lpxpak_int_t))*-1, SEEK_END)
         == -1 )
          return NULL;

     /* allocate __LPXPAK_STOP_LEN+sizeof(__lpxpak_int_t) bytes from the heap,
      * assign it to tmp, read in the xpak offset plus the__LPXPAK_STOP
      * string. */
     if ( (tmp = malloc(__LPXPAK_STOP_LEN+sizeof(__lpxpak_int_t))) == NULL )
          return NULL;
     rs = read(fd, tmp, __LPXPAK_STOP_LEN+sizeof(__lpxpak_int_t)); 
     if ( rs == -1 || rs != __LPXPAK_STOP_LEN+sizeof(__lpxpak_int_t) ){
          free(tmp);
          return NULL;
     }
     
     /* check if the read in __LPXPAK_STOP string equals __LPXPAK_STOP.  If
      * not, free the allocated memory, set errno and return NULL as this is
      * an invalid xpak. */
     if ( memcmp(tmp+sizeof(__lpxpak_int_t), __LPXPAK_STOP, __LPXPAK_STOP_LEN)
         != 0 ) {
          free(tmp);
          errno = EINVAL;
          return NULL;
     }

     /* assign a pointer to the xpak offset data to xpakoffset and convert it
      * to local byte order */
     xpakoffset = (__lpxpak_int_t *)tmp;
     *xpakoffset = ntohl(*xpakoffset);
        
     /* allocate <xpakoffset> bytes on the heap, assign it to xpakdata, seek
      * to the start of the xpak data, read in the xpak data and store it in
      * xpakdata. */
     if ( lseek(fd, (off_t)((off_t)*xpakoffset+__LPXPAK_OFFSET)*-1, SEEK_END)
          == -1 ) {
          free(tmp);
          free(xpakdata);
          return NULL;
     }
     if ( (xpakdata = malloc((size_t)*xpakoffset)) == NULL ) {
          free(tmp);
          return NULL;
     }
     rs = read(fd, xpakdata, (size_t)*xpakoffset);
     /* check if a error occured while data was read in */
     if ( rs == -1 ) {
          free(tmp);
          free(xpakdata);
          return NULL;
     }
     /* check if all of the data was read, if not, set errno and return with
      * failure  */
     if ( rs != (size_t)*xpakoffset) {
          free(tmp);
          free(xpakdata);
          errno = EBUSY;
          return NULL;
     }

     xpak = lpxpak_parse_data(xpakdata, (size_t)(*xpakoffset));
     
     /* clear up allocated memory*/
     free(tmp);
     free(xpakdata);
     return xpak;
}

/* 
 * lpxpak_parse_file: Reads the xpak data out of a FILE * buffer which points
 *                    to an Gentoo binary package.
 *
 * Gets an FILE buffer for a Gentoo binary package and returns an pointer to
 * an lpxpak object with the xpak data. If an error occurs, NULL is returned
 * and errno is set to indicate the error.
 *
 * Errors:
 *         EINVAL The file either is no valid gentoo binary package or has an
 *                invalid xpak.
 *                
 *         EBUSY The xpak could not be fully read in
 *
 *         The lpxpak_parse_file() function may also fail and set errno for
 *         any of the errors specified for the routine malloc(3).
 *
 *         The lpxpak_parse_file() function may also fail and set errno for
 *         any of the errors specified for the routine lseek(2).
 *
 *         The lpxpak_parse_file() function may also fail and set errno for
 *         any of the errors specified for the routine read(2).
 *
 *         The lpxpak_parse_file() function may also fail and set errno for
 *         any of the errors specified for the routine fileno(3).
 *
 *         The lpxpak_parse_file() function may also fail and set errno for any
 *         of the errors specified for the routine fstat(2).
 */
lpxpak_t *
lpxpak_parse_file(FILE *file)
{
     int fd;
     lpxpak_t *xpak = NULL;

     if ( (fd = fileno(file)) == -1 )
          return NULL;
     xpak = lpxpak_parse_fd(fd);
     close(fd);
     return xpak;
}

/* 
 * lpxpak_parse_path: Reads the xpak data out of a Gentoo binary packages'
 *                    path it was called with.
 *
 * Gets an path to a Gentoo binary package and returns an pointer to an lpxpak
 * object with the xpak data. If an error occurs, NULL is returned and errno
 * is set to indicate the error.
 *
 * Errors:
 *         EINVAL The file either is no valid gentoo binary package or has an
 *                invalid xpak.
 *                
 *         EBUSY The xpak could not be fully read in
 *
 *         The lpxpak_parse_path() function may also fail and set errno for
 *         any of the errors specified for the routine malloc(3).
 *
 *         The lpxpak_parse_path() function may also fail and set errno for
 *         any of the errors specified for the routine lseek(2).
 *
 *         The lpxpak_parse_path() function may also fail and set errno for
 *         any of the errors specified for the routine read(2).
 *
 *         The lpxpak_parse_path() function may also fail and set errno for
 *         any of the errors specified in the routine open(2).
 *
 *         The lpxpak_parse_path() function may also fail and set errno for any
 *         of the errors specified for the routine fstat(2).
 */
lpxpak_t *
lpxpak_parse_path(const char *path)
{
     int fd;
     lpxpak_t *xpak = NULL;

     if ( (fd = open(path, O_RDONLY)) == -1)
          return NULL;
     xpak = lpxpak_parse_fd(fd);
     close(fd);
     return xpak;
}

/*
 * __lpxpak_parse_index: parses an data block according to the provided index
 *
 * Gets an pointer to the index block of an xpak and the length of that index
 * block and returns and returns an pointer to an __lpxpak_index_t object with
 * the xpak index data. If an error occurred, NULL is returned and errno is
 * set to indicate the error.
 *
 * PRIVATE: This is a private function and thus should not be called
 *          directly from outside the API, as the way this function works
 *          can be changed regularly.
 *
 * Errors: 
 *         The __lpxpak_parse_index() function may fail and set errno for any
 *         of the errors specified for the routine malloc(3).
 */
static __lpxpak_index_t *
__lpxpak_parse_index(const void *data, size_t len)
{
     __lpxpak_index_t *index = NULL;
     __lpxpak_index_t *t = NULL;
     __lpxpak_int_t count = 0;
     __lpxpak_int_t name_len = 0;
     size_t indexsize;

     indexsize = sizeof(__lpxpak_index_t);

     /* iterate over the index block  */
     while (count < len) {
          /* check if index is initialized, if not, allocate indexsize bytes
           * on the heap and assign it to index and t, otherwise assign that
           * memory to t->next and t->next to t */
          if (index == NULL) {
               if ( (index = malloc(indexsize)) == NULL )
                    return NULL;
               __lpxpak_init_index(index);
               t = index;
          }
          else {
               if( (t->next = malloc(indexsize)) == NULL )
                    return NULL;
               __lpxpak_init_index(t->next);
               t = t->next;
          }

          /* read name_len from data and increase the counter */
          name_len = *(__lpxpak_int_t *)(data+count);
          name_len = ntohl(name_len);
          count += sizeof(__lpxpak_int_t);

          /* allocate name_len+1 bytes on the heap, assign it to t->name, read
           * name_len bytes from data into t->name, apply name_len+1 to
           * t->name_len, and increase the counter by name_len bytes */
          if ( (t->name = malloc((size_t)name_len+1)) == NULL )
               return NULL;
          memcpy(t->name, data+count, name_len);
          (t->name)[name_len] = '\0';
          count += name_len;
          
          /* read t->offset from data in local byte order and increase
           * counter */
          t->offset = *(__lpxpak_int_t *)(data+count);
          t->offset = ntohl(t->offset);
          count += sizeof(__lpxpak_int_t);

          /* read t->len from data in local byte order and increase counter */
          t->len = *(__lpxpak_int_t *)(data+count);
          t->len = htonl(t->len);
          count += sizeof(__lpxpak_int_t);
     }
     return index;
}

/*
 * __lpxpak_parse_data: parses an data block according to the provided index
 *
 * Gets an pointer to the data block of an xpak and a pointer to the index of
 * the same xpak as an __lpxpak_index_t struct and returns and returns an
 * pointer to an lpxpak_t object with the xpak data. If an error occurred,
 * NULL is returned and errno is set to indicate the error.
 *
 * PRIVATE: This is a private function and thus should not be called
 *          directly from outside the API, as the way this function works
 *          can be changed regularly.
 *
 * Errors: 
 *         The __lpxpak_parse_data() function may fail and set errno for any
 *         of the errors specified for the routine malloc(3).
 */
static lpxpak_t *
__lpxpak_parse_data(const void *data, __lpxpak_index_t *index)
{
     __lpxpak_index_t *ti = NULL;
     lpxpak_t *xpak = NULL;
     lpxpak_t *tx = NULL;

     /* operate over all index elements */
     for (ti = index; ti->next != NULL; ti = ti->next) {
          /* check if xpak is initialized, if not, allocate xpaksize bytes
           * on the heap and assign it to xpak and tx, otherwise assign that
           * memory to tx->next and tx->next to tx */
          if (xpak == NULL) {
               if ( (xpak = malloc(xpaksize)) == NULL )
                    return NULL;
               __lpxpak_init(xpak);
               tx=xpak;
          } else {
               if ( (tx->next = malloc(xpaksize)) == NULL )
                    return NULL;
               __lpxpak_init((lpxpak_t *)tx->next);
               tx = (lpxpak_t *)tx->next;
          }
          /* allocate ti->name_len bytes on the heap, assign it to tx->name
           * and copy ti->name_len bytes from ti->name to tx->name */
          if ( (tx->name = malloc((size_t)ti->name_len)) == NULL )
               return NULL;
          memcpy((lpxpak_t *)tx->name, ti->name, ti->name_len);

          /* allocate ti->len bytes on the heap, assign it to tx->value, copy
           * ti->len data from data+offset to tx->value and null-terminate
           * tx->value  */
          if ( (tx->value = malloc((size_t)ti->len)) == NULL )
               return NULL;
          memcpy((lpxpak_t *)tx->value, data+ti->offset, ti->len);
          tx->value_len = ti->len;
     }
     return xpak;
}


/*
 * __lpxpak_destroy_index: destroy an __lpxpak_index_t object
 *
 * Gets an pointer to an __lpxpak_index_t object and free(2)s up all memory of
 * that object. If a NULL pointer was given, __lpxpak_destroy_index will just
 * return.
 *
 * PRIVATE: This is a private function and thus should not be called directly
 *          from outside the API, as the way this function works can be
 *          changed regularly.
 *
 * ATTENTION: Do not try to use a destroyed __lpxpak_index_t object or
 *            unexpected things will happen
 */
static void
__lpxpak_destroy_index(__lpxpak_index_t *index)
{
     __lpxpak_index_t *t = NULL;

     t=index;

     /* iterate over the whole index and free(2) every single object */
     while ( index != NULL ) {
          /* assign index->next to t, free up index and assign t to index */
          t=index->next;
          free(index->name);
          free(index);
          index=t;
     }
     return;
}


/*
 * lpxpak_destroy_xpak: destroy an xpak object
 *
 * Gets an pointer to an lpxpak_t object and free(2)s up all memory of that
 * object. If a NULL pointer was given, lpxpak_destroy_xpak will just return.
 *
 * ATTENTION: Do not try to use a destroyed xpak object or unexpected things
 *            will happen.
 */
void
lpxpak_destroy_xpak(lpxpak_t *xpak)
{
     lpxpak_t *t = NULL;

     /* iterate over the whole index and free(2) every single object */
     while ( xpak != NULL ) {
          /* assign xpak->next to t, free up xpak and assign t to xpak */
          free((lpxpak_t *)xpak->name);
          free((lpxpak_t *)xpak->value);
          t=(lpxpak_t *)xpak->next;
          free(xpak);
          xpak=t;
     }
     return;
}


/*
 * __lpxpak_init_index: initialize an __lpxpak_index_t object
 *
 * Gets an pointer to an lpxpak_index_t object and sets all of its pointers to
 * NULL. If a NULL pointer was given, __lpxpak_init_index will just return.
 *
 * PRIVATE: This is a private function and thus should not be called directly
 *          from outside the API, as the way this function works can be
 *          changed regularly.
 */ 
static void
__lpxpak_init_index(__lpxpak_index_t *index)
{
     if (index != NULL) {
          index->name = NULL;
          index->next = NULL;
     }
     return;
}

/*
 * __lpxpak_init: initialize an xpak object
 *
 * Gets an pointer to an lpxpak_t object and sets all of its pointers to
 * NULL. If a NULL pointer was given, __lpxpak_init will just return.
 * 
 * PRIVATE: This is a private function and thus should not be called directly
 *          from outside the API, as the way this function works can be
 *          changed regularly.
 */
static void
__lpxpak_init(lpxpak_t *xpak)
{
     if (xpak != NULL) {
          xpak->name = NULL;
          xpak->value_len = 0;
          xpak->value = NULL;
          xpak->next = NULL;
     }
     return;
}
