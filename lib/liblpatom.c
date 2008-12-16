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

#include "liblpatom.h"

#include <stdlib.h>
#include <sys/types.h>
#include <regex.h>
#include <errno.h>
#include <string.h>

typedef struct {
     char *suf;
     char *rel;
} __lpatom_suf_t;

static __lpatom_suf_t *
__lpatom_suffix_parse(const char *s);

lpatom_t *
lpatom_parse(const char *s)
{
     regex_t regexp;
     regmatch_t regmatch[4];
     regoff_t rm_so, rm_eo;
     char *suff;
     lpatom_t *atom;
     __lpatom_suf_t *suf;
     char *name_re =
          "([-_+a-z0-9]+)-([0-9\\.]+[a-zA-Z]?)((_(alpha|beta|pre|rc|p)[0-9]+)?(-r[0-9]*)?)?";

     atom = (lpatom_t *)malloc(sizeof(lpatom_t));

     regcomp (&regexp, name_re, REG_EXTENDED);

     /* check if this is a valid ebuild version atom */
     if (! (regexec(&regexp, s, 4, regmatch, 0) == 0)) {
          errno = EINVAL;
          return NULL;
     }

     /* copy name string*/
     rm_so = regmatch[1].rm_so;
     rm_eo = regmatch[1].rm_eo;
     atom->name = (char *)malloc(sizeof(char)*((rm_eo-rm_so)+1));
     memcpy(atom->name, s+rm_so, rm_eo-rm_so);
     atom->name[rm_eo-rm_so] = '\0';

     /* copy version string*/
     rm_so = regmatch[2].rm_so;
     rm_eo = regmatch[2].rm_eo;
     atom->version = (char *)malloc(sizeof(char)*((rm_eo-rm_so)+1));
     memcpy(atom->version, s+rm_so, rm_eo-rm_so);
     atom->version[rm_eo-rm_so] = '\0';

     /* check if this atom has a suffix */
     if (regmatch[3].rm_so != -1 && regmatch[3].rm_eo != -1) {
          /* copy suffix string */
          rm_so = regmatch[3].rm_so;
          rm_eo = regmatch[3].rm_eo;
          suff = (char *)malloc(sizeof(char)*((rm_eo-rm_so)+1));
          memcpy(suff, s+rm_so, rm_eo-rm_so);
          suff[rm_eo-rm_so] = '\0';
     }

     suf = __lpatom_suffix_parse(suff);
     
     return atom;
}


static __lpatom_suf_t *
__lpatom_suffix_parse(const char *s)
{
     char *suf_re = "^_((alpha|beta|pre|rc|p)([0-9]+))";
     char *rel_re = "-r([0-9]+)$";
     regmatch_t regmatch[2];
     regoff_t rm_so, rm_eo;
     regex_t regexp;
     __lpatom_suf_t *suf;

     suf = (__lpatom_suf_t *)malloc(sizeof(__lpatom_suf_t));


     regcomp (&regexp, suf_re, REG_EXTENDED);
     if (regexec(&regexp, s, 2, regmatch, 0) == 0) {
          rm_so = regmatch[1].rm_so;
          rm_eo = regmatch[1].rm_eo;
          suf->suf = (char *)malloc(sizeof(char)*((rm_eo-rm_so)+1));
          memcpy(suf->suf, s+rm_so, rm_eo-rm_so);
          suf->suf[rm_eo-rm_so] = '\0';
     }

     regcomp(&regexp, rel_re, REG_EXTENDED);
     if (regexec(&regexp, s, 2, regmatch, 0) == 0) {
          rm_so = regmatch[1].rm_so;
          rm_eo = regmatch[1].rm_eo;
          suf->rel = (char *)malloc(sizeof(char)*((rm_eo-rm_so)+1));
          memcpy(suf->rel, s+rm_so, rm_eo-rm_so);
          suf->rel[rm_eo-rm_so] = '\0';
     }
     return NULL;
}
