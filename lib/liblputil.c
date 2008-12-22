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

#include "liblputil.h"
#include <string.h>
#include <errno.h>
#include <stdlib.h>



char *
lputil_get_re_match(const regmatch_t *match, int n, const char *s)
{
     if (match == NULL|| s == NULL) {
          errno = EINVAL;
          return NULL;
     }
     regoff_t rm_so, rm_eo;

     rm_so = match[n].rm_so;
     rm_eo = match[n].rm_eo;

     return lputil_substr(s, rm_so, rm_eo-rm_so);
}

char *
lputil_substr(const char *s, size_t os, size_t oe)
{
     char *r;
     if ( s == NULL || strlen(s) < (os+oe) ) {
          errno = EINVAL;
          return NULL;
     }
     if ( (r = (char *)malloc(sizeof(char)*(oe+1) )) == NULL)
          return NULL;

     memcpy(r, s+os, oe);
     r[oe] = '\0';
     return r;
}

char **
lputil_splitstr(const char *s, const char *delim)
{
     char **r;
     char *t = (char *)s;
     size_t len = 10;
     int i;
     
     if ( (r = (char **)malloc(sizeof(char *)*len)) == NULL)
          return NULL;
     for (i=0; (r[i] = strtok(t, delim)) != NULL; ++i) {
          if (t != NULL)
               t = NULL;
          if (i == len-2) {
               len += 5;
               if ( (r = (char **)realloc(r, sizeof(char *)*len)) == NULL) {
                    free(r);
                    return NULL;
               }
          }
     }
     if ( (r=realloc(r, sizeof(char *)*(i+1))) == NULL) {
          free(r);
          return NULL;
     }
     return r;
}
