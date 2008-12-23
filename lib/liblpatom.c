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

#include "liblpatom.h"
#include "liblputil.h"

#include <stdlib.h>
#include <sys/types.h>
#include <regex.h>
#include <errno.h>
#include <string.h>


#define __LP_ATOM_RE    "^(([A-Za-z0-9+_][A-Za-z0-9+_.-]*)/?([A-Za-z0-9+_-]*[A-Za-z+_-]|[A-Za-z+_][0-9]+))-([0-9]+([.][0-9]+)*[a-z]?)((_(alpha|beta|pre|rc|p)[0-9]*)?(-r[0-9]+)?)?$"
#define __LP_SUF_RE     "((_(alpha|beta|pre|rc|p)[0-9]*)?(-r[0-9]+)?)$"
#define __LP_VSUF_RE    "^_((alpha|beta|pre|rc|p)[0-9]*)"
#define __LP_REL_RE     "-r([0-9]+)$"
#define __LP_VER_RE     "([0-9]+([.][0-9]+)*)([a-z]?)"
#define __LP_VER_SUF_RE "([a-z])$"


typedef struct {
     char *suf;
     lpatom_suf_t se;
     int rel;
} __lpatom_suf_t;

static __lpatom_suf_t *
__lpatom_parse_suffix(const char *s);

static void
__lpatom_init_suffix(__lpatom_suf_t *suf);

static void
__lpatom_init(lpatom_t *atom);

static void
__lpatom_destroy_suffix(__lpatom_suf_t *suffix);

static int *
__lpatom_parse_version(const char *v);

/* 
 * lpatom_parse: Reads the atom data out of an packetname string
 *
 * Gets an version string and returns an pointer to an lpatom_t struct. If an
 * error occured, Null is returned and errno is set to indicate the error.
 *
 * Errors:
 *         EINVAL The file either is no valid gentoo binary package or has an
 *                invalid xpak.
 *
 *         The lpatom_parse() function may also fail and set errno for any of
 *         the errors specified for the routine malloc(3).
 */
lpatom_t *
lpatom_parse(const char *s)
{
     regex_t *regexp = NULL;
     regmatch_t regmatch[5];
     char *ssuf = NULL;
     char *ver, *vers;
     int count = 1;
     lpatom_t *atom = NULL;
     __lpatom_suf_t *suf = NULL;

     /* get enough memory for the atom object and initialize it */
     if ( (atom = (lpatom_t *)malloc(sizeof(lpatom_t))) == NULL)
          return NULL;
     __lpatom_init(atom);

     /* get enough memory for the regex object and use it to compile the
      * __LP_ATOM_RE regexp */
     if ( (regexp = (regex_t *)malloc(sizeof(regex_t))) == NULL)
          return NULL;
     regcomp (regexp, __LP_ATOM_RE, REG_EXTENDED);

     /* check if this is a valid ebuild version atom by regexp matching */
     if (! (regexec(regexp, s, 5, regmatch, 0) == 0)) {
          errno = EINVAL;
          return NULL;
     }
     /* clean up the regexp object */
     regfree(regexp);

     /* assign the <count>th match of the previously applied regexp to
      * atom->qname */
     if ( (atom->qname = lputil_get_re_match(regmatch, count, s)) == NULL)
          return NULL;
     ++count;

     /* check if the packet has a category prefix and assign that to atom->cat
      * if possitive */
     if(strstr(s, "/") != NULL) {
          if ((atom->cat = lputil_get_re_match(regmatch, count, s))
              ==NULL)
               return NULL;
          ++count;
     }
     
     /* assign the <count>th match of the previously applied regexp to
      * atom->name */
     if ((atom->name = lputil_get_re_match(regmatch, count, s)) == NULL)
          return NULL;
     ++count;

     /* assign the <count>th match of the previously applied regexp (The
      * package version to atom->ver */
     if ((ver = lputil_get_re_match(regmatch, count, s)) == NULL)
          return NULL;

     regcomp(regexp, __LP_VER_RE, REG_EXTENDED);
     regexec(regexp, ver, 2, regmatch, 0);
     /* assign the first match of the previously applied regexp (The version
      * number without the suffix to ver, parse ver and assign the resulting
      * int-array to atom->ver */
     if ( (vers = lputil_get_re_match(regmatch, 1, ver)) == NULL)
          return NULL;
     atom->ver = __lpatom_parse_version(vers);

     /* free up the regexp object */
     regfree(regexp);

     regcomp(regexp, __LP_VER_SUF_RE, REG_EXTENDED);
     if( regexec(regexp, ver, 2, regmatch, 0) == 0) {
          if ( (ver = lputil_get_re_match(regmatch, 1, ver)) == NULL)
               return NULL;
          atom->verc = ver[0];
     }

      /* compile __LP_SUF_RE regexp, check if it matches and assign the
       * matched string to ssuf if so */
     regcomp (regexp, __LP_SUF_RE, REG_EXTENDED);
     if ( regexec(regexp, s, 5, regmatch, 0) == 0 ) {
          if ( (ssuf = lputil_get_re_match(regmatch, 1, s)) == NULL )
               return NULL;
     }

     /* free up the regexp object */
     regfree(regexp);
     free(regexp);

     /* parse the suffix and assign the values to the atom object */
     suf = __lpatom_parse_suffix(ssuf);
     free(ssuf);
     atom->sufenum = suf->se;
     atom->rel = suf->rel;

     /* clean up the suf object  */
     __lpatom_destroy_suffix(suf);
     
     return atom;
}

/* 
 * __lpatom_parse_suffix: Reads the suffix data out of an suffix string.
 *
 * Gets an suffix string and returns an pointer to an __lpatom_suf_t
 * object. If an error occurred, NULL is returned and errno is set to indicate
 * the error.
 *
 * PRIVATE: This is a private function and thus should not be called
 *          directly from outside the API, as the way this function works
 *          can be changed regularly.
 *
 * Errors:
 *         The __lpatom_parse_suffix() function may also fail and set errno
 *         for any of the errors specified for the routine malloc(3).
 */
static __lpatom_suf_t *
__lpatom_parse_suffix(const char *s)
{
     regmatch_t regmatch[2];
     regex_t *regexp;
     char *sufs;
     char *rs;

     __lpatom_suf_t *suf;

     /* allocate memory for the suf object and initialize it */
     if ( (suf = (__lpatom_suf_t *)malloc(sizeof(__lpatom_suf_t))) == NULL)
          return NULL;
     __lpatom_init_suffix(suf);

     /* allocate memory for the compiled regexp */
     if ( (regexp = (regex_t *)malloc(sizeof(regex_t))) == NULL)
          return NULL;

     /* compile the regexp with __LP_VSUF_RE, check if it matches and assign
      * the matched string (actually the suffix) to suf->suf */
     regcomp (regexp, __LP_VSUF_RE, REG_EXTENDED);
     if (regexec(regexp, s, 2, regmatch, 0) == 0)
          if ( (sufs = lputil_get_re_match(regmatch, 1, s)) == NULL)
               return NULL;
     
     switch(sufs[0]) {
     case 'a':
          suf->se = alpha;
          break;
     case 'b':
          suf->se = beta;
          break;
     case 'p':
          switch(sufs[1]) {
          case 'r':
               suf->se = pre;
               break;
          default:
               suf->se = p;
          }
          break;
     case 'r':
          suf->se = rc;
          break;
     default:
          break;
     }
     /* clean up the compiled regexp */
     regfree(regexp);

     /* compile the regexp with __LP_REL_RE, check if it matches, convert the
      * matched string to decimal and assign it to suf->rel  */
     regcomp(regexp, __LP_REL_RE, REG_EXTENDED);
     if (regexec(regexp, s, 2, regmatch, 0) == 0) {
          if ( (rs = lputil_get_re_match(regmatch, 1, s)) == NULL)
               return NULL;
          suf->rel = atoi(rs);
     }

     /* clean up */
     regfree(regexp);
     free(regexp);
     free(rs);
     
     return suf;
}

/*
 * __lpatom_init_suffix: initialize an __lpatom_suf_t object
 *
 * Gets an pointer to an lpatom_suf_t object and sets all of its pointers to
 * NULL. If a NULL pointer was given, __lpatom_init_suffix will just return.
 *
 * PRIVATE: This is a private function and thus should not be called directly
 *          from outside the API, as the way this function works can be
 *          changed regularly.
 */
static void
__lpatom_init_suffix(__lpatom_suf_t *suf)
{
     if (suf != NULL) {
          suf->suf = NULL;
          suf->se = no;
          suf->rel = 0;
     }
     return;
}

/*
 * __lpatom_init: initialize an __lpatom_t object
 *
 * Gets an pointer to an lpatom_suf_t object and sets all of its pointers to
 * NULL. If a NULL pointer was given, __lpatom_init will just return.
 *
 * PRIVATE: This is a private function and thus should not be called directly
 *          from outside the API, as the way this function works can be
 *          changed regularly.
 */
static void
__lpatom_init(lpatom_t *atom)
{
     if(atom != NULL) {
          atom->name = NULL;
          atom->qname = NULL;
          atom->cat = NULL;
          atom->ver = NULL;
          atom->verc = 0;
          atom->suffix = NULL;
          atom->sufenum = no;
          atom->sufv = 0;
          atom->rel = 0;
     }
     return;
}

/*
 * __lpatom_destroy_suffix: destroy an __lpatom_suffix_t object
 *
 * Gets an pointer to an __lpatom_suffix_t object and free(2)s up all memory
 * of that object. If a NULL pointer was given, __lpatom_destroy_suffix will
 * just return.
 *
 * PRIVATE: This is a private function and thus should not be called directly
 *          from outside the API, as the way this function works can be
 *          changed regularly.
 *
 * ATTENTION: Do not try to use a destroyed __lpatom_suffix_t object or
 *            unexpected things will happen
 */
static void
__lpatom_destroy_suffix(__lpatom_suf_t *suffix)
{
     if (suffix != NULL ){
          free(suffix);
     }
     return;
}

/*
 * lpatom_destroy: destroy an lpatom_t object
 *
 * Gets an pointer to an lpatom_t object and free(2)s up all memory of that
 * object. If a NULL pointer was given, lpatom_destroy will just return.
 *
 * ATTENTION: Do not try to use a destroyed __lpatom_t object or unexpected
 *            things will happen
 */
void
lpatom_destroy(lpatom_t *atom)
{
     if (atom != NULL) {
          free(atom->name);
          free(atom->qname);
          free(atom->cat);
          free(atom->ver);
          free(atom->suffix);
          free(atom);
     }
     return;
}

static int *
__lpatom_parse_version(const char *v)
{
     char **p;
     int *r, i, len=10;

     p = lputil_splitstr(v, ".");
     r = malloc(sizeof(int)*len);
     for (i=0; p[i] != NULL; ++i) {
          if (i == len-1) {
               len +=5;
               if ( (r = (int *)realloc(r, sizeof(int)*len)) == NULL) {
                    free(r);
                    return NULL;
               }
          }
          r[i] = atoi(p[i]);
     }
     r[i] = -1;
     if ( (r = (int *)realloc(r, sizeof(int)*(i+1))) == NULL) {
          free(r);
          return NULL;
     }
     return r;
}
