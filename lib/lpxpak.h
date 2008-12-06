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
 * ARISING IN ANY WAY OUT OF THE USE * OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
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


#define _LP_XPAK_STOP_OFFSET_   4
#define _LP_XPAK_OFFSET_        8
#define _LP_XPAK_INTRO_LEN_     8
#define _LP_XPAK_INTRO_         "XPAKPACK"
#define _LP_XPAK_OUTRO_LEN_     8
#define _LP_XPAK_OUTRO_         "XPAKSTOP"
#define _LP_XPAK_STOP_LEN_      4
#define _LP_XPAK_STOP_          "STOP"

typedef uint32_t lpxpak_int;

typedef struct _lpxpak_{
     char *name;
     char *value;
     struct _lpxpak_ *next;
} lpxpak_t;

typedef struct _lpxpakindex_{
     char *name;
     size_t name_len;
     uint32_t offset;
     uint32_t len;
     struct _lpxpakindex_ *next;
} lpxpakindex_t;

lpxpak_t *
lpxpak_parse_data(const void *data, size_t len);

lpxpak_t *
lpxpak_parse_fd(int fd);

lpxpak_t *
lpxpak_parse_file(FILE *file);

lpxpak_t *
lpxpak_parse_path(const char *path);

lpxpakindex_t *
lpxpak_get_index(const void *data, size_t len);

lpxpak_t *
lpxpak_get_data(const void *data, lpxpakindex_t *index);

lpxpak_t *
lpxpak_parse_data(const void *data, size_t len)
{
     lpxpak_int count = 0;
     lpxpak_int index_len, data_len;
     const void *index_data, *data_data;
     lpxpakindex_t *index;
        
     /* check if the first _LP_XPAK_INTRO_LEN_ bytes of the xpak data read
      * _LP_XPAK_INTRO_ and the last _LP_XPAK_OUTRO_LEN_ bytes of of the data
      * read _LP_XPAK_OUTRO_ to make sure we have a valid xpak, afterward
      * increase count which we will be using as an seek counter */
     if ((memcmp(data, _LP_XPAK_INTRO_, _LP_XPAK_INTRO_LEN_) != 0) ||
         (memcmp(data+len-8, _LP_XPAK_OUTRO_, _LP_XPAK_OUTRO_LEN_ != 0))) {
          errno = EINVAL;
          return NULL;
     }
     count += _LP_XPAK_INTRO_LEN_;

     memcpy(&index_len, data+count, sizeof(lpxpak_int));
     count+=sizeof(lpxpak_int);
     index_len = ntohl(index_len);
     memcpy(&data_len, data+count, sizeof(lpxpak_int));
     count += sizeof(lpxpak_int);
     data_len = ntohl(data_len);

     /* check if the sum of count, index_len, data_len and _LP_XPAK_OUTRO_LEN
      * are equal to len to make sure the len values are correct */
     if (count+index_len+data_len+_LP_XPAK_OUTRO_LEN_ != len) {
          errno = EINVAL;
          return NULL;
     }

     index_data = data+count;
     data_data = data+count+index_len;

     index = lpxpak_get_index(index_data, (size_t)index_len);
     return lpxpak_get_data(data_data, index);
}

lpxpak_t *
lpxpak_parse_fd(int fd)
{
     struct stat xpakstat;
     void *xpakdata;
     void *tmp;
     lpxpak_int xpakoffset;
     lpxpak_t *xpak;
        
     if (fstat(fd, &xpakstat) == -1)
          return NULL;
        
     if (! S_ISREG(xpakstat.st_mode) ) {
          errno = EINVAL;
          return NULL;
     }

     /* seek to the start of the STOP string */
     if (lseek(fd, _LP_XPAK_STOP_OFFSET_*-1, SEEK_END) == -1)
          return NULL;
        
     /* allocate 4bytes from the heap, assign it to tmp, read in the STOP
      * string, check if the read in data is "STOP"(encoded as an ASCII
      * String) - otherwise this would be an invalid xpak, free the memory
      * that we assigned to tmp and set tmp to NULL */
     if ( (tmp = malloc(_LP_XPAK_STOP_LEN_)) == NULL)
          return NULL;
     read(fd, tmp, _LP_XPAK_STOP_LEN_);
     if (memcmp(tmp, _LP_XPAK_STOP_, _LP_XPAK_STOP_LEN_) != 0) {
          free(tmp);
          tmp = NULL;
          errno = EINVAL;
          return NULL;
     }
     free(tmp);
     tmp = NULL;
        
     /* seek to the start of the xpak_offset value, read in the offset and
      * convert it to local byteorder */
     if (lseek(fd, _LP_XPAK_OFFSET_*-1, SEEK_END) == -1)
          return NULL;
     if (read(fd, &xpakoffset, sizeof(lpxpak_int)) == -1)
          return NULL;
     xpakoffset = ntohl(xpakoffset);
        
     /* allocate <xpakoffset> bytes on the heap, assign it to xpakdata, seek
      * to to the start of the xpak data, read in the xpak data and store it
      * in xpakdata. */
     if ( (xpakdata = malloc(xpakoffset)) == NULL)
          return NULL;
     if (lseek(fd, (off_t)(xpakoffset+_LP_XPAK_OFFSET_)*-1, SEEK_END) == -1)
          return NULL;
     if (read(fd, xpakdata, (size_t)xpakoffset) == -1)
          return NULL;
     xpak = lpxpak_parse_data(xpakdata, (size_t)xpakoffset);
     free(xpakdata);
     return xpak;
}

lpxpak_t *
lpxpak_parse_file(FILE *file)
{
     int fd;

     if ( (fd = fileno(file)) == -1)
          return NULL;
     return lpxpak_parse_fd(fd);
}

lpxpak_t *
lpxpak_parse_path(const char *path)
{
     int fd;

     fd = open(path, O_RDONLY);
     return lpxpak_parse_fd(fd);
}

lpxpakindex_t *
lpxpak_get_index(const void *data, size_t len)
{
     lpxpakindex_t *index, *t;
     lpxpak_int count = 0;
     lpxpak_int name_len = 0;

     if ( (index = (lpxpakindex_t *)malloc(sizeof(lpxpakindex_t))) == NULL)
          return NULL;
     index->next = NULL;
     t=index;

     /* Do this until we reach the end of the index blob  */
     while (count < len) {
          /* read sizeof(lpxpak_int) bytes from data into name_len, convert it
           * to host byteorder and increase the counter by
           * sizeof(lpxpak_int) */
          name_len = *(lpxpak_int *)(data+count);
          name_len = ntohl(name_len);
          count += sizeof(lpxpak_int);

          /* allocate name_len+1 bytes on the heap, assign it to t->name, read
           * name_len bytes from data into t->name, apply name_len+1 to
           * t->name_len, null terminate t->name and increase the counter by
           * name_len bytes */
          if ( (t->name = (char *)malloc((size_t)name_len)) == NULL)
               return NULL;
          memcpy(t->name, data+count, name_len);
          t->name_len = name_len;
          t->name[name_len] = '\0';
          count += name_len;
          
          /* read t->offset from data in local byteorder and increase counter */
          t->offset = *(lpxpak_int *)(data+count);
          t->offset = ntohl(t->offset);
          count += sizeof(lpxpak_int);

          /* read t->len from data in local byteorder and increase counter */
          t->len = *(lpxpak_int *)(data+count);
          t->len = htonl(t->len);
          count += sizeof(lpxpak_int);

          /* allocate sizeof(lpxpakindex_t) bytes on the heap, assign it to
           * t->next, set t to t->next and set t->next to NULL */
          if ((t->next = (lpxpakindex_t *)malloc(sizeof(lpxpakindex_t)))==NULL)
               return NULL;
          t = t->next;
          t->next = NULL;
     }
     return index;
}

lpxpak_t *
lpxpak_get_data(const void *data, lpxpakindex_t *index)
{
     lpxpakindex_t *ti;
     lpxpak_t *xpak;
     lpxpak_t *tx;
     
     if ( (xpak = (lpxpak_t *)malloc(sizeof(lpxpak_t))) == NULL )
          return NULL;
     tx = xpak;
     
     /* operate over all index elements */
     for (ti = index; ti->next != NULL; ti = ti->next) {
          /* allocate ti->name_len bytes on the heap, assign it to tx->nem and
           * copy ti->name_len bytes from ti->nem to tx->name */
          if ( (tx->name = (char *)malloc((size_t)ti->name_len)) == NULL )
               return NULL;
          memcpy(tx->name, ti->name, ti->name_len);

          /* allocate ti->len+1 bytes on the heap, assign it to tx->value,
           * copy tx->len data from data+offset to tx->value and
           * null-terminate tx->value  */
          if ( (tx->value = (char *)malloc((size_t)ti->len+1)) == NULL )
               return NULL;
          memcpy(tx->value, data+ti->offset, ti->len);
          tx->value[ti->len] = '\0';
          /* allocate sizeof(lpxpak_t) bytes on the hap, assign it to
           * tx->next, set tx to tx->next and tx->next to NULL  */
          if ( (tx->next = (lpxpak_t *)malloc(sizeof(lpxpak_t))) == NULL )
               return NULL;
          tx = tx->next;
     }
     return xpak;
}
