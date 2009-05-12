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
#define _XOPEN_SOURCE   600

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <util.h>

#include <limits.h>

#if HAVE_ERRNO_H
#  include <errno.h>
#endif /* HAVE_ERRNO_H */
#ifndef errno
/* Some Systems #define this! */
extern int errno
#endif

#if STDC_HEADERS
#  include <stdlib.h>
#  include <string.h>
#elif HAVE_STRINGS_H
#  include <string.h>
#endif /* STDC_HEADERS */

#if HAVE_MEMORY_H
#  include <memory.h>
#endif /* HAVE_MEMORY_H */

#ifdef __cplusplus
extern "C" {
#endif

extern char *
lputil_get_re_match(const regmatch_t *match, int n, const char *s)
{
     /* check if one of the given pointers is NULL  */
     if ( match == NULL || s == NULL ) {
          errno = EINVAL;
          return NULL;
     }
     /* get the substring and return it  */
     return lputil_substr(s, match[n].rm_so, match[n].rm_eo-match[n].rm_so);
}

extern char *
lputil_substr(const char *s, size_t off, size_t len)
{
     char *r;
     /* check if the given pointer is NULL  */
     if ( s == NULL ) {
          errno = EINVAL;
          return NULL;
     }
     /* allocate oe+1 bytes on the Heap and assign a pointer to that region to
      * r */
     if ( (r = malloc(len+1)) == NULL )
          return NULL;

     /* copy oe bytes starting from s+os to r, null terminate r and return the
      * created c string */
     memcpy(r, s+off, len);
     r[len] = '\0';
     return r;
}

extern void *
lputil_memdup(const void *s, size_t len)
{
     void *r;

     /* check if the given pointer is NULL */
     if ( s==NULL ) {
          errno = EINVAL;
          return NULL;
     }

     /* allocate len bytes on the heap and assign a pointer to that region to
      * r*/
     if ( ( r = malloc(len)) == NULL )
          return NULL;
     /* copy len bytes from s to r and return r */
     memcpy(r, s, len);
     return r;
}

extern char **
lputil_splitstr(const char *s, const char *delim)
{
     char **r, **rt, *st, *stt;
     unsigned int i, size = 10;
     size_t dlen = strlen(delim);

     /* allocate memory */
     if ( (r = malloc(sizeof(char *)*size)) == NULL )
          return NULL;
     if ( (st = strdup(s)) == NULL )
          return NULL;

     /* iterate over chrchr matches */
     for ( i=0; (stt = strstr(st, delim)) != NULL; ++i ) {
          /* check if we got enough space left in r */
          if ( i == (size-1) ) {
               /* allocate some more space */
               size += 5;
               if ( (rt = realloc(r, size)) == NULL ) {
                    lputil_splitstr_destroy(r);
                    return NULL;
               }
               r = rt;
          }
          /* set the start of the found delimiter to \0 */
          stt[0] = '\0';
          /* assign the first character after the previously found delimiter
           * to r[i] */
          r[i] = st;
          /* assign the first character after the currently found delimiter to
           * st */
          st = stt+dlen;
     }
     /* assign the first character after the last delimiter to r[i] */
     r[i] = st;
     /* shrink r to optimal size */
     if ( i < (size) ) {
          if ( (rt = realloc(r, sizeof(char *)*(i+2))) == NULL ) {
               lputil_splitstr_destroy(r);
               return NULL;
          }
          r = rt;
     }
     /* NULL terminate r */
     r[i+1] = NULL;
     
     return r;
}

extern size_t
lputil_intlen(int d)
{
     size_t i = 0;

     /* avoid overflow on negation: value range -2^n .. (2^n)-1 */
     if ( d == INT_MIN ) {
          d = INT_MAX;
          ++i;
     }
     if ( d < 0 ) {
          d = -d;
          ++i;
     }

     while ( d >= 10 ) {
          ++i;
          d /= 10;
     }
     /* we still got one digit left */
     ++i;
     return i;
}

extern void
lputil_splitstr_destroy(char **splitstr)
{
     free(splitstr[0]);
     free(splitstr);
}

#ifdef __cplusplus
}
#endif
