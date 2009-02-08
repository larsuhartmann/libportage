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
#define _XOPEN_SOURCE   600

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <liblputil.h>

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

BEGIN_C_DECLS

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
     char **r;
     char **rt;
     char *t;
     size_t len = 128;
     int i, j;

     if ( s == NULL || delim == NULL ) {
          errno = EINVAL;
          return NULL;
     }
     
     if ( (t = strdup(s)) == NULL )
          return NULL;
     
     /* allocate len bytes on the heap and assign a pointer to that to that
      * region r */
     if ( (r = malloc(sizeof(char *)*len)) == NULL )
          return NULL;

     /* iterate over all strtok matches */
     for ( i=0; (r[i] = strtok(t, delim)) != NULL; ++i ) {
          if ( t != NULL )
               t = NULL;
          /* check if we still got enough space in r, if not reallocate it
           * with space for 5 more strings */
          if ( i == len-2 ) {
               len += 128;
               /* reallocate the region r points to and assign a pointer to
                * that region to r, if not successfull, clean up everything we
                * ve allocated so far and return */
               if ( (rt = realloc(r, sizeof(char *)*len)) == NULL ) {
                    /* iterate over r and free up every entry */
                    for ( j=0; j <= i; ++j )
                         free(r[j]);
                    free(r);
                    free(t);
                    return NULL;
               }
               /* assign the pointer to the reallocated region(rt) to r */
               r = rt;
          }
     }
     /* reallocate r to the optimal size */
     if ( (rt = realloc(r, sizeof(char *)*(i+1))) == NULL ) {
          /* iterate over r and free up every entry */
          for ( j=0; j < i; ++j )
               free(r[j]);
          free(r);
          free(t);
          return NULL;
     }
     r = rt;
     r[i] = NULL;
     /* we do not need to NULL terminate the r, as strtok's last result was
      * already NULL */
     free(t);
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

extern char *
lputil_strndup(char *s, size_t n)
{
     char *d, *dt;
     size_t len;

     if ( (d = malloc(n)) == NULL )
          return NULL;

     strncpy(d, s, n);
     if ( ( len = strlen(d)) < n-1)
          if ( (dt = realloc(d, len+1)) == NULL ) {
               free(d);
               return NULL;
          }
     d = dt;
     return d;
}

END_C_DECLS