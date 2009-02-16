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

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <xpak.h>
#include <util.h>

#include <sys/stat.h>
#include <arpa/inet.h>

#include <fcntl.h>

#if HAVE_UNISTD_H
#  include <unistd.h>
#endif /* HAVE_UNISTD_H */

#if HAVE_ERRNO_H
#  include <errno.h>
#endif /* HAVE_ERRNO_H */
#ifndef errno
/* Some Systems #define this! */
extern int errno
#endif /* errno */

#if STDC_HEADERS
#  include <stdlib.h>
#  include <string.h>
#elif HAVE_STRINGS_H
#  include <string.h>
#endif /* STDC_HEADERS */

#if HAVE_MEMORY_H
#  include <memory.h>
#endif /* HAVE_MEMORY_H

/**
 * \brief The Offset for the STOP String - calculated from SEEK_END.
 */
#define LPXPAK_STOP_OFFSET      4
/**
 * \brief The Length of the XPAK_OFFSET field.
 */
#define LPXPAK_OFFSET_LEN       8
/**
 * \brief The Length of the INTRO String.
 */
#define LPXPAK_INTRO_LEN        8
/**
 * \brief The Value the INTRO String should have.
 */
#define LPXPAK_INTRO            "XPAKPACK"
/**
 * \brief The Length of the OUTRO String.
 */
#define LPXPAK_OUTRO_LEN        8
/**
 * \brief The Value the OUTRO String should have.
 */
#define LPXPAK_OUTRO            "XPAKSTOP"
/**
 * \brief The Length of the STOP String in Bytes 
 */
#define LPXPAK_STOP_LEN         4
/**
 * \brief The Value the STOP String should have.
 */
#define LPXPAK_STOP             "STOP"

/**
 * \brief size of lpxpak_int_t in bytes.
 */
#define LPXPAK_INT_SIZE       4

BEGIN_C_DECLS

/**
 * \brief The Datatype for the offset/length values XPAK uses.
 */
typedef uint32_t lpxpak_int_t;

/**
 * \brief A xpak index element.
 *
 * This is the Datastructure that holds one element of the Index Data parsed
 * by __lpxpak_parse_index.
 *
 * None of the Pointers in this struct will point to memory regions which are
 * used elsewhere by the lpxpak library.
 */
typedef struct {
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
} lpxpak_index_t;

/**
 * \brief Parses the Index block of an XPAK.
 *
 * Gets a pointer to the index block of an xpak and the length of that index
 * block and returns and returns a \c NULL terminated array of pointers
 * tolpxpak_index_t structures with the xpak index data. If an error
 * occurred, \c NULL is returned and errno is set to indicate the error.
 *
 * The provided Index data will not be modified by this function.
 *
 * \param data a pointer to a memory segment which holds the index block.
 * 
 * \param len the length of the index block pointed to by data.
 *
 * \return A \c NULL terminated aray of pointers to lpxpak_index_t
 * structures, which hold the parsed data or \c NULL if an error occured.
 *
 * \sa lpxpak_index_destroy()
 *
 * \b Errors:
 * 
 * - This function may fail and set errno for any of the errors specified for
 *   the routine malloc(3).
 */
static lpxpak_index_t **
lpxpak_index_parse(const void *data, size_t len);

/**
 * \brief Allocates a new \c NULL terminated array of pointers to
 * lpxpak_index_t structures.
 *
 * If an error occurs, \c NULL is returned and errno is set.
 *
 * The returned data structure can be initialized by lpxpak_index_init().
 * 
 * The Memory for the returned array including all its members can be freed
 * with lpxpak_index_destroy().
 *
 * \param size the size of the requested array.
 * 
 * \return a \c NULL terminated array of lpxpak_index_t or \c NULL if an
 * error occured.
 *
 * \sa lpxpak_index_init() lpxpak_index_destroy()
 * 
 * \b Errors:
 *
 * - This function may fail and set errno for any of the errors specified for
 *   the routine malloc(3).
 */
static lpxpak_index_t **
lpxpak_index_create(size_t size);

/**
 * \brief Initialises a \c NULL terminated array of pointers to lpxpak_index_t
 * structures with the length \c size.
 *
 * This function sets all values of the structure to \c 0 and all pointers to
 * \c NULL.
 *
 * \param index a \c NULL terminated array of pointers to lpxpak_index_t
 * structs.
 * 
 * \sa lpxpak_index_init() lpxpak_index_destroy()
 */
static void
lpxpak_index_init(lpxpak_index_t **index);

/**
 * \brief resize a \c NULL terminated array of pointers to lpxpak_index_t
 * structures.
 *
 * resizes the array and frees all unused structs if the new size is lower, or
 * allocate more space for structs if the size is higher. If the new size is
 * higher, the values and pointers of the newly allocated structs are set to
 * \c 0 or \c NULL. If a \c NULL pointer was given, this function will just
 * return. If an error occurs, \c NULL is returned and errno is set to
 * indicate the error. If the resize fails, index is untouched.
 *
 * \param index a \c NULL terminated array of lpxpak_index_t structures which
 * is to be resized
 * \param size the new size.
 * 
 * \return the resized array or \c NULL if an error occured.
 *
 * \sa lpxpak_index_destroy()
 *
 * \b Errors:
 *
 * - \c EINVAL The given pointer was NULL.
 * - This function may also fail and set errno for any of the errors specified
 *   for the routine realloc(3)
 */
static lpxpak_index_t **
lpxpak_index_resize(lpxpak_index_t **index, size_t size);

/**
 * \brief Destroy an \c NULL terminated array of pointers to lpxpak_index_t
 * structures.
 *
 * frees up the array and all of the underlying structures plus the pointers
 * they have using free(3). If a \c NULL pointer was given, this function will
 * just return.
 *
 * \param index a \c NULL terminated array of pointers to lpxpak_index_t
 * structures.
 *
 * \b Attention: Do not try to access the array after destroying it or
 * anything can happen.
 */
static void
lpxpak_index_destroy(lpxpak_index_t **index);

/**
 * \brief Parses an data block according to the provided index
 *
 * Gets a pointer to the data block of an xpak and a \c NULL terminated array
 * of pointers to lpxpak_index_t structures which hold the corresponding
 * index data and returns and returns a \c NULL terminated array of pointers
 * to lpxpak_t structures with the xpak data. If an error occurred, \c NULL is
 * returned and errno is set to indicate the error.
 *
 * The provided lpxpak_index_t may be modified by this function.
 *
 * \param data a pointer to the memory segment which holds the data block of
 * the xpak.
 *
 * \param index a \c NULL terminated array of lpxpak_index_t structures
 * which hold the corresponding index data.
 *
 * \return a \c NULL terminated array of lpxpak_index_t structures which
 * hold the parsed xpak data or \c NULL if an error occured.
 *
 * \sa lpxpak_destroy()
 *
 * \b Errors:
 *
 * - This function may fail and set errno for any of the errors specified for
 *   the routine malloc(3).
 *
 * - This function may fail and set errno for any of the errors specified for
 *   the routine strdup(3).
 */

static lpxpak_t **
lpxpak_data_parse(const void *data, lpxpak_index_t **index);

/**
 * \brief Allocates a new \c NULL terminated array of pointers to lpxpak_t
 * structures with the length \c size.
 *
 * If an error occurs, \c NULL is returned and errno is set.
 *
 * The returned data structure can be initialized by lpxpak_init().
 * 
 * The Memory for the returned array including all its members can be freed
 * with lpxpak_destroy().
 *
 * \param size the size of the requested array.
 *
 * \return a \c NULL terminated array of lpxpak_t structures or \c NULL if an
 * error occured.
 *
 * \sa lpxpak_init() lpxpak_destroy()
 * 
 * \b Errors:
 *
 * - This function may fail and set errno for any of the errors specified for
 * the routine malloc(3).
 */
 static lpxpak_t **
 lpxpak_create(size_t size);

/**
 * \brief Initialises a \c NULL terminated array of pointers to lpxpak_t
 * structures.
 *
 * This function sets all values of the structures to \c 0 and all pointers to
 * \c NULL.
 *
 * \param xpak a \c NULL terminated array of lpxpak_t structures.
 *
 * \sa lpxpak_create() lpxpak_destroy()
 * 
 * \b Errors:
 *
 * - This function may fail and set errno for any of the errors specified for
 * the routine malloc(3).
 */
static void
lpxpak_init(lpxpak_t **);

/**
 * \brief compiles the index data of a \c NULL terminated array of lpxpak_t
 * structs into a xpak index binary blob.
 *
 * If an error occurs, \c NULL is returned and \c errno is set to indicate the
 * error.
 *
 * \param xpak a \c NULL terminated array of lpxpak_t data structures.
 *
 * \return a pointer to a lpxpak_blob_t data structure or \c NULL if an error
 * occured.
 * 
 * \b Errors:
 * 
 * - This function may fail and set errno for any of the errors specified for
 *   the routine malloc(3).
 */
static lpxpak_blob_t *
lpxpak_indexblob_compile(lpxpak_t **xpak);

/**
 * \brief compiles the data data of a \c NULL terminated array of lpxpak_t
 * structs into a xpak data binary blob.
 *
 * If an error occurs, \c NULL is returned and \c errno is set to indicate the
 * error.
 *
 * \param xpak a \c NULL terminated array of lpxpak_t data structures.
 *
 * \return a pointer to a lpxpak_blob_t data structure or \c NULL if an error
 * occured.
 * 
 * \b Errors:
 * 
 * - This function may fail and set errno for any of the errors specified for
 *   the routine malloc(3).
 */
static lpxpak_blob_t *
lpxpak_datablob_compile(lpxpak_t **xpak);

extern lpxpak_t **
lpxpak_parse_data(const lpxpak_blob_t *blob)
{
     lpxpak_int_t count = 0;
     lpxpak_int_t index_len, data_len;
     const void *index_data = NULL;
     const void *data_data = NULL;
     lpxpak_index_t **index = NULL;
     size_t tl = 0;
     lpxpak_t **xpak = NULL;
     int i;

     /* check if the first __LPXPAK_INTRO_LEN bytes of the xpak data read
      * __LPXPAK_INTRO and the last __LPXPAK_OUTRO_LEN bytes of the xpak read
      * __LPXPAK_OUTRO to make sure we have a valid xpak, if not, set errno
      * and return. Otherwise increase count which we will be using as an seek
      * counter */
     if ( (memcmp(blob->data, LPXPAK_INTRO, LPXPAK_INTRO_LEN) != 0) ||
         (memcmp((uint8_t *)blob->data+blob->len-LPXPAK_OUTRO_LEN,
         LPXPAK_OUTRO, LPXPAK_OUTRO_LEN != 0)) ) {
          errno = EINVAL;
          return NULL;
     }
     count += LPXPAK_INTRO_LEN;
     
     memcpy(&index_len, (uint8_t *)blob->data+count, LPXPAK_INT_SIZE);
     count+=LPXPAK_INT_SIZE;
     index_len = ntohl(index_len);
     memcpy(&data_len, (uint8_t *)blob->data+count, LPXPAK_INT_SIZE);
     count += LPXPAK_INT_SIZE;
     data_len = ntohl(data_len);

     /* check if the sum of count, index_len, data_len and __LPXPAK_OUTRO_LEN
      * is equal to len to make sure the len values are correct */
     if ( count+index_len+data_len+LPXPAK_OUTRO_LEN != blob->len ) {
          errno = EINVAL;
          return NULL;
     }

     index_data = (uint8_t *)blob->data+count;
     data_data = (uint8_t *)blob->data+count+index_len;

     /* get index */
     index = lpxpak_index_parse(index_data, (size_t)index_len);
     /* check if the sum of all len entries of all data elements is equal to
      * data_len to make sure the len values are correct, if not, clean up the
      * heap, set errno and return.  */
     for ( i=0; index[i] != NULL; ++i )
          tl += index[i]->len;
     if ( tl != data_len ) {
          errno = EINVAL;
          return NULL;
     }

     /* get xpak-data  */
     xpak = lpxpak_data_parse(data_data, index);

     /* free up index */
     lpxpak_index_destroy(index);

     return xpak;
}

extern lpxpak_t **
lpxpak_parse_fd(int fd)
{
     struct stat xpakstat;
     lpxpak_blob_t *xpakblob;
     lpxpak_t **xpak;

     /* check if the given fd belongs to a file */
     if ( fstat(fd, &xpakstat) == -1 )
          return NULL;
     if (! S_ISREG(xpakstat.st_mode) ) {
          errno = EBADFD;
          return NULL;
     }

     /* get the xpakblob from lpxpak_get_blob_fd */
     if ( (xpakblob = lpxpak_blob_get_fd(fd)) == NULL )
          return NULL;

     /* parse the xpakblob */
     xpak = lpxpak_parse_data(xpakblob);
     /* free up xpakblob */
     lpxpak_blob_destroy(xpakblob);
     /* return whatever lpxpak_parse has returned (could be NULL or a pointer,
      * does not matter to us here */
     return xpak;
}

extern lpxpak_blob_t *
lpxpak_blob_get_fd(int fd)
{
     void *xpakdata = NULL;
     void *tmp = NULL;
     lpxpak_int_t *xpakoffset = NULL;
     ssize_t rs;
     lpxpak_blob_t *xpakblob;

     /* seek to the start of the xpak offset */
     if ( lseek(fd, (LPXPAK_STOP_OFFSET+LPXPAK_INT_SIZE)*-1, SEEK_END)
         == -1 )
          return NULL;

     /* allocate __LPXPAK_STOP_LEN+LPXPAK_INT_T_SIZE bytes from the heap,
      * assign it to tmp, read in the xpak offset plus the__LPXPAK_STOP
      * string. */
     if ( (tmp = malloc(LPXPAK_STOP_LEN+LPXPAK_INT_SIZE)) == NULL )
          return NULL;
     if ( (rs = read(fd, tmp, LPXPAK_STOP_LEN+LPXPAK_INT_SIZE))
         == -1 ) {
          free(tmp);
          return NULL;
     }
     if ( rs != LPXPAK_STOP_LEN+LPXPAK_INT_SIZE ) {
          free(tmp);
          errno = EBUSY;
          return NULL;
     }
     
     /* check if the read in __LPXPAK_STOP string equals __LPXPAK_STOP.  If
      * not, free the allocated memory, set errno and return NULL as this is
      * an invalid xpak. */
     if ( memcmp((uint8_t *)tmp+LPXPAK_INT_SIZE, LPXPAK_STOP,
         LPXPAK_STOP_LEN) != 0 ) {
          free(tmp);
          errno = EINVAL;
          return NULL;
     }
     /* assign a pointer to the xpak offset data to xpakoffset and convert it
      * to local byte order */
     xpakoffset = (lpxpak_int_t *)tmp;
     *xpakoffset = ntohl(*xpakoffset);

     /* allocate <xpakoffset> bytes on the heap, assign it to xpakdata, seek
      * to the start of the xpak data, read in the xpak data and store it in
      * xpakdata. */
     if ( lseek(fd, (off_t)((off_t)*xpakoffset+LPXPAK_OFFSET_LEN)*-1,
         SEEK_END) == -1 ) {
          free(tmp);
          free(xpakdata);
          return NULL;
     }
     if ( (xpakdata = malloc((size_t)*xpakoffset)) == NULL ) {
          free(tmp);
          return NULL;
     }
     /* check if a error occured while data was read in */
     if ( (rs = read(fd, xpakdata, (off_t)*xpakoffset)) == -1 ) {
          free(tmp);
          free(xpakdata);
          return NULL;
     }
     /* check if all of the data was read, if not, set errno and return with
      * failure  */
     if ( rs != (size_t)*xpakoffset ) {
          free(tmp);
          free(xpakdata);
          errno = EBUSY;
          return NULL;
     }
     /* initialize data structure for xpakblob */
     if ( (xpakblob = lpxpak_blob_create()) == NULL ) {
          free(tmp);
          free(xpakdata);
          return NULL;
     }
     lpxpak_blob_init(xpakblob);
     xpakblob->data = xpakdata;
     xpakblob->len = *xpakoffset;
     
     /* clear up allocated memory*/
     free(tmp);
     return xpakblob;
}

extern lpxpak_t **
lpxpak_parse_path(const char *path)
{
     int fd;
     lpxpak_t **xpak = NULL;

     /* open file descriptor in read only mode and call lpxpak_parse_fd */
     if ( (fd = open(path, O_RDONLY)) == -1 )
          return NULL;
     xpak = lpxpak_parse_fd(fd);
     /* close file descriptor and return the pointer we got from
      * lpxpak_parse_fd */
     close(fd);
     return xpak;
}

static lpxpak_index_t **
lpxpak_index_parse(const void *data, size_t len)
{
     lpxpak_index_t **index = NULL;
     lpxpak_index_t **t = NULL;
     lpxpak_int_t count = 0;
     lpxpak_int_t name_len = 0;
     size_t intlen = 100;
     int i;

     /* initialize a array of lpxpak_index_t structs with len entries */
     if ( (index = lpxpak_index_create(intlen)) == NULL )
          return NULL;
     lpxpak_index_init(index);
     
     /* iterate over the index block  */
     for ( i=0; count < len; ++i ) {
          if ( i == intlen-1 ) {
               intlen += 50;
               /* check if we got enough free entries in index, if not resize
                * it */
               if ( (t = lpxpak_index_resize(index, intlen)) == NULL ) {
                    lpxpak_index_destroy(index);
                    return NULL;
               }
               index = t;
               t = NULL;
          }
          /* read name_len from data and increase the counter */
          name_len = *((lpxpak_int_t *)((uint8_t *)data+count));
          name_len = ntohl(name_len);
          count += LPXPAK_INT_SIZE;

          /* allocate name_len+1 bytes on the heap, assign it to t->name, read
           * name_len bytes from data into t->name, apply name_len+1 to
           * t->name_len, and increase the counter by name_len bytes */
          if ( (index[i]->name = malloc((size_t)name_len+1)) == NULL ) {
               lpxpak_index_destroy(index);
               return NULL;
          }
          memcpy(index[i]->name, (uint8_t *)data+count, name_len);
          (index[i]->name)[name_len] = '\0';
          count += name_len;
          
          /* read t->offset from data in local byte order and increase
           * counter */
          index[i]->offset = *((lpxpak_int_t *)((uint8_t *)data+count));
          index[i]->offset = ntohl(index[i]->offset);
          count += LPXPAK_INT_SIZE;

          /* read t->len from data in local byte order and increase counter */
          index[i]->len = *((lpxpak_int_t *)((uint8_t *)data+count));
          index[i]->len = htonl(index[i]->len);
          count += LPXPAK_INT_SIZE;
     }
     /* now that we know the actual size, resize index to that size */
     if ( (t = lpxpak_index_resize(index, i)) == NULL ) {
          lpxpak_index_destroy(index);
          return NULL;
     }
     index = t;
     return index;
}

static lpxpak_index_t **
lpxpak_index_create(size_t size)
{
     lpxpak_index_t **index, *mem;
     int i;

     /* allocate memory */
     if ( (index = malloc(sizeof(lpxpak_index_t *)*(size+1))) == NULL )
          return NULL;
     if ( (mem = malloc(sizeof(lpxpak_index_t)*size)) == NULL ) {
          free(index);
          return NULL;
     }

     /* assign memory */
     for ( i=0; i < size; ++i )
          index[i] = mem+i;
     /* NULL terminate the array */
     index[size] = NULL;
     
     return index;
}

static void
lpxpak_index_init(lpxpak_index_t **index)
{
     int i;

     for ( i=0; index[i] != NULL; ++i ) {
          index[i]->name = NULL;
          index[i]->offset = 0;
          index[i]->len = 0;
     }
     return;
}

static lpxpak_index_t **
lpxpak_index_resize(lpxpak_index_t **index, size_t size)
{
     lpxpak_index_t **tindex = NULL;
     lpxpak_index_t *t = NULL;
     int i;
     int len;

     if ( index == NULL ) {
          errno = EINVAL;
          return NULL;
     }
     
     for ( i = 0; index[i] != NULL; ++i )
          ;
     len = i;

     /* realloc the index */
     if ( (tindex = realloc(index, sizeof(lpxpak_index_t *)*(size+1)))
         == NULL ) {
          return NULL;
     }
     index = tindex;
     /* realloc the elements */
     if ( (t = realloc(index[0], sizeof(lpxpak_index_t)*size)) == NULL )
          return NULL;
     for ( i=0; i < size; ++i )
          index[i] = t+i;
     /* initialize new memory */
     if ( len < size )
          lpxpak_index_init(index+len);
     index[size] = NULL;
     return index;
}

static void
lpxpak_index_destroy(lpxpak_index_t **index)
{
     int i;

     if ( index == NULL )
          return;
     /* iterate over the elements and free the name strings */
     for ( i=0; index[i] != NULL; ++i )
          free(index[i]->name);
     /* free the elements */
     free(index[0]);
     /* free the main index */
     free(index);
     return;
}

static lpxpak_t **
lpxpak_data_parse(const void *data, lpxpak_index_t **index)
{
     lpxpak_t **xpak = NULL;
     int i, isize, dlen = 0;
     void *tdata;

     /* count the total length */
     for ( i=0; index[i] != NULL; ++i )
          dlen += index[i]->len;
     isize = i;

     /* initialize a new empty NULL terminated array of lpxpak_t structures
      * with the size len */
     if ( (xpak = lpxpak_create(isize)) == NULL )
          return NULL;
     lpxpak_init(xpak);

     /* copy the data block */
     if ( (tdata = lputil_memdup(data, dlen)) == NULL ) {
          lpxpak_destroy(xpak);
          return NULL;
     }
     
     /* operate over all index elements */
     for ( i=0; i < isize; ++i ) {
          /* assign ti->name to tx->name and set ti->name to NULL */
          xpak[i]->name = index[i]->name;
          index[i]->name = NULL;

          /* assign the index[i]->offset'th byte of tdata to xpak[i]->value */
          xpak[i]->value = (uint8_t*)tdata + index[i]->offset;
          xpak[i]->value_len = index[i]->len;
     }
     return xpak;
}

static lpxpak_t **
lpxpak_create(size_t size)
{
     lpxpak_t **xpak, *mem;
     int i;

     /* allocate memory */
     if ( (xpak = malloc(sizeof(lpxpak_t *)*(size+1))) == NULL )
          return NULL;
     if ( (mem = malloc(sizeof(lpxpak_t)*size)) == NULL ) {
          free(xpak);
          return NULL;
     }
     /* assign memory */
     for ( i=0; i<size; ++i )
          xpak[i] = mem+i;
     /* NULL terminate the array */
     xpak[size] = NULL;
     return xpak;
}

static void
lpxpak_init(lpxpak_t ** xpak)
{
     int i;

     /* set all pointers to NULL and all values to 0 */
     for ( i=0; xpak[i] != NULL; ++i ) {
          xpak[i]->name = NULL;
          xpak[i]->value = NULL;
          xpak[i]->value_len = 0;
     }
     return;
}

extern void
lpxpak_destroy(lpxpak_t **xpak)
{
     int i;

     if ( xpak == NULL )
          return;

     /* iterate over all entrys and free the name strings */
     for ( i=0; xpak[i] != NULL; ++i )
          free(xpak[i]->name);
     /* free the data */
     free(xpak[0]->value);
     /* free the elements */
     free(xpak[0]);
     /* free the main xpak */
     free(xpak);
     return;
}

extern lpxpak_blob_t *
lpxpak_blob_create(void)
{
     return malloc(sizeof(lpxpak_blob_t));
}

extern void
lpxpak_blob_init(lpxpak_blob_t *blob)
{
     /* initialize data */
     blob->data = NULL;
     blob->len = 0;
     return;
}

extern void
lpxpak_blob_destroy(lpxpak_blob_t *blob)
{
     if ( blob == NULL )
          return;
     free(blob->data);
     free(blob);
}

extern lpxpak_t *
lpxpak_get(lpxpak_t **xpak, char *key)
{
     int i;

     /* iterate over xpak until the end (NULL) or the searched entry was found
      * and return the last processed entry which should be either NULL or the
      * one we searched for */
     for ( i=0; xpak[i] != NULL || strcmp(xpak[i]->name, key) != 0; ++i )
          ;
     return xpak[i];
}

lpxpak_blob_t *
lpxpak_blob_get_path(const char *path)
{
     int fd;
     lpxpak_blob_t *xpakblob = NULL;

     /* open file descriptor in read only mode and call lpxpak_parse_fd */
     if ( (fd = open(path, O_RDONLY)) == -1 )
          return NULL;
     xpakblob = lpxpak_blob_get_fd(fd);
     /* close file descriptor and return the pointer we got from
      * lpxpak_parse_fd */
     close(fd);
     return xpakblob;
}

extern lpxpak_blob_t *
lpxpak_blob_compile(lpxpak_t **xpak)
{
     size_t bloblen;
     size_t count = 0;
     lpxpak_blob_t *index;
     lpxpak_blob_t *data;
     lpxpak_blob_t *blob;
     lpxpak_int_t bil, bdl;
    

     /* build index blob */
     if ( (index = lpxpak_indexblob_compile(xpak)) == NULL )
          return NULL;

     if ( (data = lpxpak_datablob_compile(xpak)) == NULL ) {
          lpxpak_blob_destroy(index);
          return NULL;
     }

     bloblen = LPXPAK_INTRO_LEN + (LPXPAK_INT_SIZE*2) + data->len +
          index->len + LPXPAK_OUTRO_LEN;

     if ( (blob = lpxpak_blob_create()) == NULL ) {
          lpxpak_blob_destroy(index);
          return NULL;
     }
     lpxpak_blob_init(blob);
     if ( (blob->data = malloc(bloblen)) == NULL ) {
          lpxpak_blob_destroy(index);
          lpxpak_blob_destroy(blob);
          return NULL;
     }
     bil = htonl(index->len);
     bdl = htonl(data->len);
     
     memcpy(blob->data, LPXPAK_INTRO, LPXPAK_INTRO_LEN);
     count += LPXPAK_INTRO_LEN;
     memcpy((uint8_t *)blob->data+count, &bil, LPXPAK_INT_SIZE);
     count += LPXPAK_INT_SIZE;
     memcpy((uint8_t *)blob->data+count, &bdl, LPXPAK_INT_SIZE);
     count += LPXPAK_INT_SIZE;
     memcpy((uint8_t *)blob->data+count, index->data, index->len);
     count += index->len;
     lpxpak_blob_destroy(index);
     memcpy((uint8_t *)blob->data+count, data->data, data->len);
     count += data->len;
     lpxpak_blob_destroy(data);
     memcpy((uint8_t *)blob->data+count, LPXPAK_OUTRO, LPXPAK_OUTRO_LEN);
     count += LPXPAK_OUTRO_LEN;
     blob->len = count;

     return blob;
}

static lpxpak_blob_t *
lpxpak_datablob_compile(lpxpak_t **xpak)
{
     int i;
     lpxpak_blob_t *datablob;
     size_t count = 0;
     size_t datalen = 0;

     /* initialise the datablob data structure */
     if ( (datablob = lpxpak_blob_create()) == NULL )
          return NULL;
     lpxpak_blob_init(datablob);
     
     /* get len of the data blob */
     for ( i=0; xpak[i] != NULL; ++i )
          datalen += xpak[i]->value_len;

     /* allocate enough memory for the datablob */
     if ( (datablob->data = malloc(datalen)) == NULL ) {
          lpxpak_blob_destroy(datablob);
          return NULL;
     }
     datablob->len = datalen;
     
     /* copy data to datablob */
     for ( i=0; xpak[i] != NULL; ++i ) {
          memcpy((uint8_t *)datablob->data+count, xpak[i]->value,
                 xpak[i]->value_len);
          count += xpak[i]->value_len;
     }

     return datablob;
}

static lpxpak_blob_t *
lpxpak_indexblob_compile(lpxpak_t **xpak)
{
     int i;
     size_t count = 0;
     lpxpak_int_t indexslen;
     lpxpak_int_t offset = 0;
     lpxpak_int_t bil, bol, bsl;
     size_t index_len = 0;
     lpxpak_blob_t *index;

     for ( i=0; xpak[i] != NULL; ++i ) {
          index_len += strlen(xpak[i]->name);
          index_len += LPXPAK_INT_SIZE*3;
     }

     if ( (index = lpxpak_blob_create()) == NULL )
          return NULL;
     lpxpak_blob_init(index);
     if ( (index->data = malloc(index_len)) == NULL ) {
          lpxpak_blob_destroy(index);
          return NULL;
     }
     index->len = index_len;

     for ( i=0; xpak[i] != NULL; ++i ) {
          indexslen = (lpxpak_int_t)strlen(xpak[i]->name);
          bil = htonl(indexslen);
          memcpy((uint8_t *)index->data+count, &bil, LPXPAK_INT_SIZE);
          count += LPXPAK_INT_SIZE;
          memcpy((uint8_t *)index->data+count, xpak[i]->name, indexslen);
          count += indexslen;
          bsl = htonl(offset);
          memcpy((uint8_t *)index->data+count, &bsl, LPXPAK_INT_SIZE);
          count += LPXPAK_INT_SIZE;
          bol = htonl(xpak[i]->value_len);
          memcpy((uint8_t *)index->data+count, &bol, LPXPAK_INT_SIZE);
          offset += xpak[i]->value_len;
          count += LPXPAK_INT_SIZE;
     }
     index->len = count;
     return index;
}

END_C_DECLS
