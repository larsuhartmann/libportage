/*
 * Copyright (c) 2008-2009 Lars Hartmann
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

/**
 * \brief Feature test macro for POSIX.1, POSIX.2, XPG4, SUSv2, SUSv3.
 *
 * This makes sure, we have a Standard conformant environment.
 */
#define _XOPEN_SOURCE   600

#include "liblpxpak.h"
#include "liblputil.h"

#include <sys/stat.h>
#include <arpa/inet.h>

#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

/** \brief The Offset for the STOP String - calculated from SEEK_END. */
#define __LPXPAK_STOP_OFFSET    4
/** \brief The Length of the XPAK_OFFSET field in Bytes. */
#define __LPXPAK_OFFSET_LEN         8
/** \brief The Length of the INTRO String in Bytes. */
#define __LPXPAK_INTRO_LEN      8
/** \brief The Value the INTRO String should have. */
#define __LPXPAK_INTRO          "XPAKPACK"
/** \brief The Length of the OUTRO String in Bytes. */
#define __LPXPAK_OUTRO_LEN      8
/** \brief The Value the OUTRO String should have. */
#define __LPXPAK_OUTRO          "XPAKSTOP"
/** \brief The Length of the STOP String in Bytes  */
#define __LPXPAK_STOP_LEN       4
/** \brief The Value the STOP String should have. */
#define __LPXPAK_STOP           "STOP"

/** \brief The Datatype for the offset/length values XPAK uses. */
typedef uint32_t __lpxpak_int_t;

/**
 * \private
 * \brief The xpak index Data.
 *
 * This is the Datastructure that holds the Index Data parsed by
 * __lpxpak_parse_index. It is implemented as a single linked list. The Memory
 * for an lpxpak_t object can be freed with lpxpak_destroy_xpak()
 *
 * None of the Pointers in this struct will point to memory regions which are
 * used elsewhere by the lpxpak library.
 *
 * \sa __lpxpak_destroy_index()
 */
typedef struct __lpxpak_index{
     /**
      * \brief The Name of the Element.
      *
      * This Element is implemented as a null terminated C String.
      */
     char *name;
     /**
      * \brief The Offset of the Data.
      *
      * The Offset needed to find the corresponding data block, calculated
      * from the start of the data block.
      */
     size_t offset;
     /**
      * \brief The Length of the Data Block.
      *
      * The Length needed to copy out the corresponding data block.
      */
     size_t len;
     /**
      * \brief A pointer to the next Element in the list.
      */
     struct __lpxpak_index *next;
} __lpxpak_index_t;

/**
 * \private
 * \brief Parses the Index block of an XPAK.
 *
 * Gets a pointer to the index block of an xpak and the length of that index
 * block and returns and returns a pointer to an __lpxpak_index_t object with
 * the xpak index data. If an error occurred, \c NULL is returned and errno is
 * set to indicate the error. 
 *
 * The provided Index data will not be modified by this function.
 *
 * \param data a pointer to a memory segment which holds the index block.
 * 
 * \param len the length of the index block pointed to by data.
 *
 * \return a pointer to a __lpxpak_index_t structure which holds the parsed
 * index data or \c NULL if an error occured.
 *
 * \sa __lpxpak_destroy_index()
 *
 * \b Errors:
 * 
 * - This function may fail and set errno for any of the errors specified for
 *   the routine malloc(3).
 */
static __lpxpak_index_t *
__lpxpak_parse_index(const void *data, size_t len);

/**
 * \private
 * \brief Parses an data block according to the provided index
 *
 * Gets a pointer to the data block of an xpak and a pointer to the
 * corresponding __lpxpak_index_t object and returns and returns a pointer to
 * an lpxpak_t object with the xpak data. If an error occurred, \c NULL is
 * returned and errno is set to indicate the error.
 *
 * The provided __lpxpak_index_t may be modified by this function.
 *
 * \param data a pointer to the memory segment which holds the data block of
 * the xpak.
 *
 * \param index a pointer to the corresponding __lpxpak_index_t structure.
 *
 * \return a pointer to a lpxpak_t structure which holds the parsed xpak data
 * or \c NULL if an error occured.
 *
 * \sa lpxpak_destroy_xpak()
 *
 * \b Errors:
 *
 * - This function may fail and set errno for any of the errors specified for
 *   the routine malloc(3).
 *
 * - This function may fail and set errno for any of the errors specified for
 *   the routine strdup(3).
 */
static lpxpak_t *
__lpxpak_parse_data(const void *data, __lpxpak_index_t *index);

/**
 * \private
 * \brief Destroy an __lpxpak_index_t object.
 *
 * Gets a pointer to an __lpxpak_index_t object and frees up all memory of
 * that object using free(3). If a \c NULL pointer was given,
 * __lpxpak_destroy_index will just return.
 *
 * \param index a pointer to the __lpxpak_index_t to be freed.
 *
 * \b Attention: Do not try to use a destroyed __lpxpak_index_t object or
 * any thing could happen.
 */
static void
__lpxpak_destroy_index(__lpxpak_index_t *index);

/**
 * \private
 * \brief Allocates and initialises a new __lpxpak_index_t struct.
 *
 * Allocates a new __lpxpak_index_t object, sets all its values to 0 and all
 * its pointers to \c NULL and returns a pointer to it, if an error occurs,
 * \c NULL is returned and errno is set.
 *
 * The Memory for an __lpxpak_index_t object can be freed with
 * __lpxpak_destroy_index().
 *
 * \return a pointer to a new __lpxpak_index_t struct or \c NULL if an error
 * occured.
 *
 * \sa __lpxpak_destroy_index()
 * 
 * \b Errors:
 *
 * - This function may fail and set errno for any of the errors specified for
 * the routine malloc(3).
 */
static __lpxpak_index_t *
__lpxpak_init_index(void);

/**
 * \private
 * \brief Allocates and initializes a new lpxpak_t struct.
 *
 * Allocates a new xpak_t object, sets all its values to 0 and all
 * its pointers to \c NULL and returns a pointer to it, if an error occurs,
 * \c NULL is returned and errno is set.
 *
 * The Memory for an xpak_t object can be freed with lpxpak_destroy_xpak()
 *
 * \return a pointer to a new xpak_t struct or \c NULL if an error
 * occured.
 *
 * \sa lpxpak_destroy_xpak()
 * 
 * \b Errors:
 *
 * - This function may fail and set errno for any of the errors specified for
 * the routine malloc(3).
 */
static lpxpak_t *
__lpxpak_init(void);

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
         (memcmp((uint8_t *)data+len-__LPXPAK_OUTRO_LEN, __LPXPAK_OUTRO,
         __LPXPAK_OUTRO_LEN != 0))) {
          errno = EINVAL;
          return NULL;
     }
     count += __LPXPAK_INTRO_LEN;

     
     memcpy(&index_len, (uint8_t *)data+count, sizeof(__lpxpak_int_t));
     count+=sizeof(__lpxpak_int_t);
     index_len = ntohl(index_len);
     memcpy(&data_len, (uint8_t *)data+count, sizeof(__lpxpak_int_t));
     count += sizeof(__lpxpak_int_t);
     data_len = ntohl(data_len);

     /* check if the sum of count, index_len, data_len and __LPXPAK_OUTRO_LEN
      * is equal to len to make sure the len values are correct */
     if ( count+index_len+data_len+__LPXPAK_OUTRO_LEN != len ) {
          errno = EINVAL;
          return NULL;
     }

     index_data = (uint8_t *)data+count;
     data_data = (uint8_t *)data+count+index_len;

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
     if ( memcmp((uint8_t *)tmp+sizeof(__lpxpak_int_t), __LPXPAK_STOP,
         __LPXPAK_STOP_LEN) != 0 ) {
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
     if ( lseek(fd, (off_t)((off_t)*xpakoffset+__LPXPAK_OFFSET_LEN)*-1,
         SEEK_END) == -1 ) {
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

static __lpxpak_index_t *
__lpxpak_parse_index(const void *data, size_t len)
{
     __lpxpak_index_t *index = NULL;
     __lpxpak_index_t *t = NULL;
     __lpxpak_int_t count = 0;
     __lpxpak_int_t name_len = 0;

     /* iterate over the index block  */
     while (count < len) {
          /* check if index is initialized, if not, allocate indexsize bytes
           * on the heap and assign it to index and t, otherwise assign that
           * memory to t->next and t->next to t */
          if (index == NULL) {
               if ( (index = __lpxpak_init_index()) == NULL)
                    return NULL;
               t = index;
          }
          else {
               if ( (t->next = __lpxpak_init_index()) == NULL) {
                    __lpxpak_destroy_index(index);
                    return NULL;
               }
               t = t->next;
          }

          /* read name_len from data and increase the counter */
          name_len = *((__lpxpak_int_t *)((uint8_t *)data+count));
          name_len = ntohl(name_len);
          count += sizeof(__lpxpak_int_t);

          /* allocate name_len+1 bytes on the heap, assign it to t->name, read
           * name_len bytes from data into t->name, apply name_len+1 to
           * t->name_len, and increase the counter by name_len bytes */
          if ( (t->name = malloc((size_t)name_len+1)) == NULL ) {
               __lpxpak_destroy_index(index);
               return NULL;
          }
          memcpy(t->name, (uint8_t *)data+count, name_len);
          (t->name)[name_len] = '\0';
          count += name_len;
          
          /* read t->offset from data in local byte order and increase
           * counter */
          t->offset = *((__lpxpak_int_t *)((uint8_t *)data+count));
          t->offset = ntohl(t->offset);
          count += sizeof(__lpxpak_int_t);

          /* read t->len from data in local byte order and increase counter */
          t->len = *((__lpxpak_int_t *)((uint8_t *)data+count));
          t->len = htonl(t->len);
          count += sizeof(__lpxpak_int_t);
     }
     return index;
}

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
               if ( (xpak = __lpxpak_init()) == NULL)
                    return NULL;
               tx=xpak;
          } else {
               if ( (tx->next = __lpxpak_init()) == NULL) {
                    lpxpak_destroy_xpak(xpak);
                    return NULL;
               }
               tx = (lpxpak_t *)tx->next;
          }
          /* assign ti->name to tx->name and set ti->name to NULL */
          tx->name = ti->name;
          ti->name = NULL;

          /* allocate ti->len bytes on the heap, assign it to tx->value, copy
           * ti->len data from data+offset to tx->value and null-terminate
           * tx->value  */
          if ( (tx->value = lputil_memdup((uint8_t *)data+ti->offset, ti->len))
               == NULL) {
               lpxpak_destroy_xpak(xpak);
               return NULL;
          }
          tx->value_len = ti->len;
     }
     return xpak;
}


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

static __lpxpak_index_t *
__lpxpak_init_index(void)
{
     __lpxpak_index_t *index;
     if ( (index = malloc(sizeof(__lpxpak_index_t))) == NULL )
          return NULL;
     index->name = NULL;
     index->next = NULL;
     return index;
}

static lpxpak_t *
__lpxpak_init(void)
{
     lpxpak_t *xpak;
     if ( (xpak = malloc(sizeof(lpxpak_t))) == NULL)
          return NULL;
     xpak->name = NULL;
     xpak->value_len = 0;
     xpak->value = NULL;
     xpak->next = NULL;
     return xpak;
}
