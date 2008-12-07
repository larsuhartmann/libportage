/*
 * Copyright (c) 2008, Lars Hartmann
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
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

#include <sys/types.h>
#include <sys/stat.h>
#include <arpa/inet.h>

#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <stdint.h>
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

typedef struct lpxpak{
     char *name;
     char *value;
     struct lpxpak *next;
} lpxpak_t;

typedef struct __lpxpak_index{
     char *name;
     size_t name_len;
     uint32_t offset;
     uint32_t len;
     struct __lpxpak_index *next;
} __lpxpak_index_t;

lpxpak_t *
lpxpak_parse_data(const void *data, size_t len);

lpxpak_t *
lpxpak_parse_fd(int fd);

lpxpak_t *
lpxpak_parse_file(FILE *file);

lpxpak_t *
lpxpak_parse_path(const char *path);

__lpxpak_index_t *
_lpxpak_parse_index_(const void *data, size_t len);

lpxpak_t *
__lpxpak_parse_data(const void *data, __lpxpak_index_t *index);

void
__lpxpak_destroy_index(__lpxpak_index_t *index);

void
lpxpak_destroy_xpak_(lpxpak_t *xpak);

/* 
 * lpxpak_parse_data: Reads the xpak data out of a xpak binary blob
 *
 * Gets the actual xpak-blob (see doc/xpak.txt) and returns an pointer to an
 * lpxpak object with its data. If an error occurs, NULL is returned and errno
 * is set to indicate the error
 *
 * Errors:
 *
 * EINVAL The provided data (*data) is no valid xpak.
 * ENOMEM Could not allocate enough memory.
 */
lpxpak_t *
lpxpak_parse_data(const void *data, size_t len)
{
     __lpxpak_int_t count = 0;
     __lpxpak_int_t index_len, data_len;
     const void *index_data = NULL;
     const void *data_data = NULL;
     __lpxpak_index_t *index = NULL;
     lpxpak_t *xpak = NULL;
     
     /* check if the first __LPXPAK_INTRO_LEN bytes of the xpak data read
      * __LPXPAK_INTRO and the last __LPXPAK_OUTRO_LEN bytes of of the xpak
      * read __LPXPAK_OUTRO to make sure we have a valid xpak, if not, set
      * errno and return. Otherwise increase count which we will be using as
      * an seek counter */
     if ((memcmp(data, __LPXPAK_INTRO, __LPXPAK_INTRO_LEN) != 0) ||
         (memcmp(data+len-8, __LPXPAK_OUTRO, __LPXPAK_OUTRO_LEN != 0))) {
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
      * are equal to len to make sure the len values are correct */
     if ( count+index_len+data_len+__LPXPAK_OUTRO_LEN != len ) {
          errno = EINVAL;
          return NULL;
     }

     index_data = data+count;
     data_data = data+count+index_len;

     index = _lpxpak_parse_index_(index_data, (size_t)index_len);
     xpak = __lpxpak_parse_data(data_data, index);

     /* free up index */
     __lpxpak_destroy_index(index);

     return xpak;
}

/* 
 * lpxpak_parse_fd: Reads the xpak data out of a file-descriptor which points
 *                  to an Gentoo binary package
 *
 * Gets an file-descriptor (fd) for a Gentoo binary package and returns an
 * pointer to an lpxpak object with the xpak data. If an error occurs, NULL is
 * returned and errno is set to indicate the error
 *
 * Errors:
 *
 * EBADF  the provided fd is bad.
 *
 * EACCES permission to access the file was denied.
 *
 * ENAMETOOLONG
 *        File name too long.
 *
 * EINVAL The fd does not point to an file,or the File does not contain a
 *        valid xpak.
 *           
 * ENOMEM Could not allocate enough memory.
 *
 * ENOTDIR
 *        A component of the path is not a directory.
 */
lpxpak_t *
lpxpak_parse_fd(int fd)
{
     struct stat xpakstat;
     void *xpakdata = NULL;
     void *tmp = NULL;
     __lpxpak_int_t *xpakoffset = NULL;
     lpxpak_t *xpak = NULL;
     
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

     /* allocate _LPXPAK_STOP_LEN_+sizeof(__lpxpak_int_t) bytes from the heap,
      * assign it to tmp, read in the xpak offset plus the_LPXPAK_STOP_
      * string. */
     if ( (tmp = malloc(__LPXPAK_STOP_LEN+sizeof(__lpxpak_int_t))) == NULL ) {
               errno = ENOMEM;
               return NULL;
     }
     if (read(fd, tmp, __LPXPAK_STOP_LEN+sizeof(__lpxpak_int_t)) == -1)
          return NULL;
     
     /* check if the read in __LPXPAK_STOP string equals _LPXPAK_STOP_.  If
      * not, free the allocated memory, set errno and return NULL as this is
      * an invalid xpak. */
     if ( memcmp(tmp+sizeof(__lpxpak_int_t), __LPXPAK_STOP, __LPXPAK_STOP_LEN)
         != 0 ) {
          free(tmp);
          errno = EINVAL;
          return NULL;
     }

     /* assign a pointer to the xpak offset data to xpakoffset and convert it
      * to local byteorder */
     xpakoffset = (__lpxpak_int_t *)tmp;
     *xpakoffset = ntohl(*xpakoffset);
        
     /* allocate <xpakoffset> bytes on the heap, assign it to xpakdata, seek
      * to to the start of the xpak data, read in the xpak data and store it
      * in xpakdata. */
     if ( (xpakdata = malloc((size_t)*xpakoffset)) == NULL ) {
          errno = ENOMEM;
          return NULL;
     }
     if ( lseek(fd, (off_t)((off_t)*xpakoffset+__LPXPAK_OFFSET)*-1, SEEK_END) == -1 )
          return NULL;
     if ( read(fd, xpakdata, (size_t)(*xpakoffset)) == -1 )
          return NULL;

     xpak = lpxpak_parse_data(xpakdata, (size_t)(*xpakoffset));
     
     /* clear up allocated memory*/
     free(tmp);
     free(xpakdata);
     return xpak;
}

/* 
 * lpxpak_parse_file: Reads the xpak data out of a FILE * buffer which points
 *                    to an Gentoo binary package
 *
 * Gets an FILE buffer for a Gentoo binary package and returns an pointer to
 * an lpxpak object with the xpak data. If an error occurs, NULL is returned
 * and errno is set to indicate the error
 *
 * Errors:
 *
 * EBADF  the provided file buffer is bad.
 *
 * EACCES permission to access the file was denied.
 *
 * ENAMETOOLONG
 *        File name too long.
 *
 * EINVAL The fd does not point to an file, or the File does not contain a
 *        valid xpak.
 *           
 * ENOMEM Could not allocate enough memory.
 *
 * ENOTDIR
 *        A component of the path is not a directory.
 */
lpxpak_t *
lpxpak_parse_file(FILE *file)
{
     int fd;

     if ( (fd = fileno(file)) == -1 )
          return NULL;
     return lpxpak_parse_fd(fd);
}

/* 
 * lpxpak_parse_path: Reads the xpak data out of a Gentoo binary package with
 *                    which's path it was called with.
 *
 * Gets an path to a Gentoo binary package and returns an pointer to an lpxpak
 * object with the xpak data. If an error occurs, NULL is returned and errno
 * is set to indicate the error
 *
 * Errors:
 *
 * EBADF  the provided file buffer is bad.
 *
 * EACCES permission to access the file was denied.
 *
 * ENAMETOOLONG
 *        File name too long.
 *
 * ELOOP  Too many symbolic links were encountered.
 *
 * ENFILE The system's limit on the total number of open files has been reached.
 *
 * EINVAL The fd does not point to an file, or the file does not contain a
 *        valid xpak.
 *           
 * ENOMEM Could not allocate enough memory.
 *
 * ENOTDIR
 *        A component of the path is not a directory.
 */
lpxpak_t *
lpxpak_parse_path(const char *path)
{
     int fd;

     if ( (fd = open(path, O_RDONLY)) == -1)
          return NULL;
     return lpxpak_parse_fd(fd);
}

__lpxpak_index_t *
_lpxpak_parse_index_(const void *data, size_t len)
{
     __lpxpak_index_t *index = NULL;
     __lpxpak_index_t *t = NULL;
     __lpxpak_int_t count = 0;
     __lpxpak_int_t name_len = 0;

     if ( (index = (__lpxpak_index_t *)malloc(sizeof(__lpxpak_index_t)))
          == NULL ) {
          errno = ENOMEM;
          return NULL;
     }
     index->next = NULL;
     t=index;

     /* Do this until we reach the end of the index blob  */
     while (count < len) {
          /* read name_len from data and increase the counter */
          name_len = *(__lpxpak_int_t *)(data+count);
          name_len = ntohl(name_len);
          count += sizeof(__lpxpak_int_t);

          /* allocate name_len+1 bytes on the heap, assign it to t->name, read
           * name_len bytes from data into t->name, apply name_len+1 to
           * t->name_len, and increase the counter by name_len bytes */
          if ( (t->name = (char *)malloc((size_t)name_len)) == NULL ) {
               errno = ENOMEM;
               return NULL;
          }
          memcpy(t->name, data+count, name_len);
          t->name_len = name_len;
          count += name_len;
          
          /* read t->offset from data in local byteorder and increase
           * counter */
          t->offset = *(__lpxpak_int_t *)(data+count);
          t->offset = ntohl(t->offset);
          count += sizeof(__lpxpak_int_t);

          /* read t->len from data in local byteorder and increase counter */
          t->len = *(__lpxpak_int_t *)(data+count);
          t->len = htonl(t->len);
          count += sizeof(__lpxpak_int_t);

          /* allocate sizeof(__lpxpak_index_t) bytes on the heap, assign it to
           * t->next, set t to t->next and set t->next to NULL */
          if ((t->next = (__lpxpak_index_t *)malloc(sizeof(__lpxpak_index_t)))
              == NULL) {
               errno = ENOMEM;
               return NULL;
          }
          t = t->next;
          t->next = NULL;
     }
     return index;
}

lpxpak_t *
__lpxpak_parse_data(const void *data, __lpxpak_index_t *index)
{
     __lpxpak_index_t *ti = NULL;
     lpxpak_t *xpak = NULL;
     lpxpak_t *tx = NULL;
     
     if ( (xpak = (lpxpak_t *)malloc(sizeof(lpxpak_t))) == NULL ) {
          errno = ENOMEM;
          return NULL;
     }
     tx = xpak;
     
     /* operate over all index elements */
     for (ti = index; ti->next != NULL; ti = ti->next) {
          /* allocate ti->name_len bytes on the heap, assign it to tx->name
           * and copy ti->name_len bytes from ti->name to tx->name */
          if ( (tx->name = (char *)malloc((size_t)ti->name_len)) == NULL ) {
               errno = ENOMEM;
               return NULL;
          }
          memcpy(tx->name, ti->name, ti->name_len);

          /* allocate ti->len bytes on the heap, assign it to tx->value, copy
           * ti->len data from data+offset to tx->value and null-terminate
           * tx->value  */
          if ( (tx->value = (char *)malloc((size_t)ti->len)) == NULL ) {
               errno = ENOMEM;
               return NULL;
          }
          memcpy(tx->value, data+ti->offset, ti->len);

          /* allocate sizeof(lpxpak_t) bytes on the heap, assign it to
           * tx->next, set tx to tx->next and tx->next to NULL  */
          if ( (tx->next = (lpxpak_t *)malloc(sizeof(lpxpak_t))) == NULL ) {
               errno = ENOMEM;
               return NULL;
          }
          tx = tx->next;
     }
     return xpak;
}

void
__lpxpak_destroy_index(__lpxpak_index_t *index)
{
     __lpxpak_index_t *t;
     t=index;
     while ( index != NULL ) {
          t=index->next;
          free(index);
          index=t;
     }
}


/*
 * lpxpak_destroy_xpak_: free() up all memory used by the xpak object given as
 *                      argument.
 *
 * ATTENTION: Do not try to use a destroye'd xpak object or unexpected things
 *            will happen.
 */
void
lpxpak_destroy_xpak_(lpxpak_t *xpak)
{
     lpxpak_t *t;
     t=xpak;
     while ( xpak != NULL ) {
          t=xpak->next;
          free(xpak);
          xpak=t;
     }
}
