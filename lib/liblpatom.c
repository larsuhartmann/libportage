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

#include <liblpatom.h>
#include <liblputil.h>

#include <stdlib.h>
#include <sys/types.h>
#include <regex.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>

/**
 * \brief Regular expression for a package atom.
 */
#define LPATOM_RE         "^(([A-Za-z0-9+_][A-Za-z0-9+_.-]*)/?([A-Za-z0-9+_-]\
*[A-Za-z+_-]|[A-Za-z+_][0-9]+))-([0-9]+([.][0-9]+)*[a-z]?)((_(alpha|beta|pre|\
rc|p)[0-9]*)?(-r[0-9]+)?)?$"
/**
 * \brief Regular expression for a package suffix
 */
#define LPATOM_RE_SUF     "((_(alpha|beta|pre|rc|p)[0-9]*)?(-r[0-9]+)?)$"
/**
 * \brief Regular expression for a package suffix suffix
 */
#define LPATOM_RE_VSUF    "^_((alpha|beta|pre|rc|p)[0-9]*)"
/**
 * \brief Regular expression to check for a suffix version
 */
#define LPATOM_RE_SUFV    "^_((alpha|beta|pre|rc|p)([0-9]+))"
/**
 * \brief Regular expression for a package release version
 */
#define LPATOM_RE_REL     "-r([0-9]+)$"
/**
 * \brief Regular expression for a Package version
 */
#define LPATOM_RE_VER     "([0-9]+([.][0-9]+)*)([a-z]?)"
/**
 * \brief Regular expression for a Version suffix
 */
#define LPATOM_RE_VER_SUF "([a-z])$"


typedef struct {
     lpatom_sufe_t se;
     int sufv;
     int rel;
} lpatom_suf_t;

static lpatom_suf_t *
lpatom_suffix_parse(const char *s);

static lpatom_suf_t *
lpatom_suffix_init(void);

static lpatom_t *
lpatom_init(void);

static void
lpatom_suffix_destroy(lpatom_suf_t *suffix);

static char *
lpatom_suffe_to_string(lpatom_sufe_t suffix);

static int *
lpatom_version_explode(const char *ver);

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
     regex_t regexp[1];
     regmatch_t regmatch[5];
     char *ssuf = NULL;
     char *ver, *vers;
     int count = 2;
     lpatom_t *atom = NULL;
     lpatom_suf_t *suf = NULL;
     bool has_cat = false;

     /* get enough memory for the atom object and initialize it */
     if ( (atom = lpatom_init()) == NULL)
          return NULL;

     /* compile the __LP_ATOM_RE regexp */
     regcomp (regexp, LPATOM_RE, REG_EXTENDED);

     /* check if this is a valid ebuild version atom by regexp matching */
     if (! (regexec(regexp, s, 5, regmatch, 0) == 0)) {
          regfree(regexp);
          errno = EINVAL;
          return NULL;
     }
     /* clean up the regexp object */
     regfree(regexp);

     /* check if the packet has a category prefix and assign that to atom->cat
      * if possitive */
     if(strstr(s, "/") != NULL) {
          if ((atom->cat = lputil_get_re_match(regmatch, count, s))
              ==NULL)
               return NULL;
          ++count;
          has_cat = true;
     }
     /* assign the <count>th match of the previously applied regexp (the name)
      * to atom->name */
     if ((atom->name = lputil_get_re_match(regmatch, count, s)) == NULL)
          return NULL;
     ++count;
     if (has_cat == false)
          ++count;     

     /* assign the <count>th match (the version part)of the previously applied
      * regexp (The package version) to ver */
     if ((ver = lputil_get_re_match(regmatch, count, s)) == NULL)
          return NULL;
     regcomp(regexp, LPATOM_RE_VER, REG_EXTENDED);
     regexec(regexp, ver, 2, regmatch, 0);
     /* assign the first match of the previously applied regexp (The version
      * number without the suffix to ver, parse ver and assign the resulting
      * int-array to atom->ver */
     if ( (vers = lputil_get_re_match(regmatch, 1, ver)) == NULL)
          return NULL;
     atom->ver = vers;
     /* free up the regexp object */
     regfree(regexp);
     /* assign the output of lpatom_version_explode to atom->ver_exploded */
     if ( (atom->ver_ex = lpatom_version_explode(vers)) == NULL) {
          free(ver);
          lpatom_destroy(atom);
          return NULL;
     }
     /* check if we have a version suffix (a-z) and if yes, add it to
      * atom->verc */
     regcomp(regexp, LPATOM_RE_VER_SUF, REG_EXTENDED);
     if( regexec(regexp, ver, 2, regmatch, 0) == 0) {
          if ( (vers = lputil_get_re_match(regmatch, 1, ver)) == NULL) {
               free(ver);
               regfree(regexp);
               return NULL;
          }
          atom->verc = vers[0];
     }
     free(vers);
     free(ver);
     regfree(regexp);

      /* compile __LP_SUF_RE regexp, check if it matches and assign the
       * matched string to ssuf if so */
     regcomp (regexp, LPATOM_RE_SUF, REG_EXTENDED);
     if ( regexec(regexp, s, 5, regmatch, 0) == 0 ) {
          if ( (ssuf = lputil_get_re_match(regmatch, 1, s)) == NULL ) {
               regfree(regexp);
               return NULL;
          }
     }

     /* free up the regexp object */
     regfree(regexp);

     /* parse the suffix and assign the values to the atom object */
     suf = lpatom_suffix_parse(ssuf);
     free(ssuf);
     atom->sufenum = suf->se;
     atom->rel = suf->rel;
     atom->sufv = suf->sufv;

     /* clean up the suf object  */
     lpatom_suffix_destroy(suf);
     
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
static lpatom_suf_t *
lpatom_suffix_parse(const char *s)
{
     regmatch_t regmatch[4];
     regex_t regexp[1];
     char *sufs;
     char *sufv;
     char *rs;
     lpatom_suf_t *suf;

     /* allocate memory for the suf object and initialize it */
     if ( (suf = lpatom_suffix_init()) == NULL)
          return NULL;

     /* compile the regexp with __LP_VSUF_RE, check if it matches and assign
      * the matched string (the suffix) to sufs */
     regcomp (regexp, LPATOM_RE_VSUF, REG_EXTENDED);
     if ( regexec(regexp, s, 2, regmatch, 0) == 0 ) {
          if ( (sufs = lputil_get_re_match(regmatch, 1, s)) == NULL)
               return NULL;
          /* simple switch cases for parsing the suffix (yeah, i know this
           * would allow other non-valid suffixes to be parsed, but this would
           * have been detected by the regexp matching earlier  */
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
               suf->se = no;
               break;
          }
          regfree(regexp);
          regcomp(regexp, LPATOM_RE_SUFV, REG_EXTENDED);
          if ( regexec(regexp, s, 4, regmatch, 0) == 0) {
               if ( (sufv = lputil_get_re_match(regmatch, 3, s)) == NULL)
                    return NULL;
               suf->sufv = atoi(sufv);
          }
     }
     free(sufs);
     /* clean up the compiled regexp */
     regfree(regexp);

     /* compile the regexp with __LP_REL_RE, check if it matches, convert the
      * matched string to decimal and assign it to suf->rel  */
     regcomp(regexp, LPATOM_RE_REL, REG_EXTENDED);
     if (regexec(regexp, s, 2, regmatch, 0) == 0) {
          if ( (rs = lputil_get_re_match(regmatch, 1, s)) == NULL)
               return NULL;
          suf->rel = atoi(rs);
          free(rs);
     }
     /* clean up */
     regfree(regexp);
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
static lpatom_suf_t *
lpatom_suffix_init(void)
{
     lpatom_suf_t *suf;
     if ( (suf = malloc(sizeof(lpatom_suf_t))) == NULL )
          return NULL;
     suf->se = no;
     suf->rel = 0;
     return suf;
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
static lpatom_t *
lpatom_init(void)
{
     lpatom_t *atom;
     
     if ( (atom = malloc(sizeof(lpatom_t))) == NULL)
          return NULL;
     atom->name = NULL;
     atom->cat = NULL;
     atom->ver = NULL;
     atom->verc = 0;
     atom->sufenum = no;
     atom->sufv = 0;
     atom->rel = 0;

     return atom;
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
lpatom_suffix_destroy(lpatom_suf_t *suffix)
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
          free(atom->cat);
          free(atom->ver);
          free(atom->ver_ex);
          free(atom);
     }
     return;
}

char *
lpatom_get_suffix(const lpatom_t *atom)
{
     char *suf;
     char *r;
     char *sufv = NULL;
     size_t sufvl;
     size_t len;
     if ( (suf = lpatom_suffe_to_string(atom->sufenum)) == NULL)
          return NULL;
     len = strlen(suf);
     if ( atom->sufv > 0 ) {
          sufvl = lputil_intlen(atom->sufv)+1;;
          len += sufvl;
          if ( (sufv = malloc(sufvl)) == NULL) {
               free(suf);
               return NULL;
          }
          snprintf(sufv, sufvl, "%d", atom->sufv);
     }
     ++len;
     if ( (r = malloc(len)) == NULL) {
          free(suf);
          return NULL;
     }
     strcpy(r, suf);
     if (sufv != NULL) {
          strncat(r, sufv, len);
          free(sufv);
     }
     free(suf);
     return r;
}

char *
lpatom_get_release(const lpatom_t *atom)
{
     char *r;
     char *t;
     if (atom->rel > 0) {
          if ( (t = malloc(sizeof(char)*12)) == NULL )
               return NULL;
          sprintf(t, "r%d", atom->rel);
     } else
          if ( (t = strdup("")) == NULL)
               return NULL;
     r = strdup(t);
     free(t);
     return r;
}

char *
lpatom_get_qname(const lpatom_t *atom)
{
     char *cat, *name, *r;
     size_t len = 0;
     if (atom->cat != NULL) {
          len = strlen(atom->cat);
          if ( (cat = malloc(sizeof(char)*(len+2))) == NULL )
               return NULL;
          memcpy(cat, atom->cat, len);
          cat[len] = '/';
          cat[len+1] = '\0';
          len += 2;
     } else
          if ( (cat = strdup("")) == NULL)
               return NULL;
     if ( (name = strdup(atom->name)) == NULL)
          return NULL;
     len += strlen(name);
     if ( (r = malloc(sizeof(char)*len)) == NULL )
          return NULL;
     strcpy(r, cat);
     strcat(r, name);
     free(cat);
     free(name);
     return r;
}

static char *
lpatom_suffe_to_string(lpatom_sufe_t suffix)
{
     char *suf;
     
     switch(suffix){
     case alpha:
          if ( (suf = strdup("alpha")) == NULL)
               return NULL;
          break;
     case beta:
          if ( (suf = strdup("beta")) == NULL)
               return NULL;
          break;
     case pre:
          if ( (suf = strdup("pre")) == NULL)
               return NULL;
          break;
     case rc:
          if ( (suf = strdup("rc")) == NULL)
               return NULL;
          break;
     case p:
          if ( (suf = strdup("p")) == NULL)
               return NULL;
          break;
     default:
          if ( (suf = strdup("")) == NULL)
               return NULL;
          break;
     }
     return suf;
}

char *
lpatom_get_version(const lpatom_t *atom)
{
     size_t len, lent = 0;
     char *r;
     
     len = strlen(atom->ver);
     lent = len;
     if (atom->verc > 0) {
          lent = len;
          ++len;
     }
     if ( (r = malloc(len+1)) == NULL)
          return NULL;
     strcpy(r, atom->ver);
     if (lent != 0)
          r[lent] = atom->verc;
     r[len] = '\0';
     return r;
}

static int *
lpatom_version_explode(const char *ver) {
     int *ia;
     size_t i;
     char **sv;

     /* split up the version string */
     if ( (sv = lputil_splitstr(ver, ".")) == NULL)
          /* return NULL if error was received */
          return NULL;
     /* get the number of individual strings */
     for (i=0; sv[i] != NULL; ++i)
          ;
     /* allocate space for an int array with i entrys */
     if ( (ia = malloc(sizeof(int)*(i+1))) == NULL) {
          /* if allocation wasnt successful, throw away sv and return NULL
           * (errno is already set by malloc) */
          lputil_splitstr_destroy(sv);
          return NULL;
     }
     /* iterate over the string array which was returned by
      * lputil_splitstr() */
     for (i=0; sv[i] != NULL; ++i)
          /* convert the current string to an int and assign that one to
           * ia[i] */
          ia[i] = atoi(sv[i]);
     /* clean up the string array which was returned by lputil_splitstr() */
     ia[i] = -1;
     lputil_splitstr_destroy(sv);
     return ia;
}


int
lpatom_version_cmp(const lpatom_t *atom1, const lpatom_t *atom2)
{
     int *ia1, *ia2;
     int i;

     ia1 = atom1->ver_ex;
     ia2 = atom2->ver_ex;
     
     /* iterate over both int arrays until the end of one of em is reached */
     for (i=0; ia1[i] != -1 || ia2[i] != -1; ++i)
          /* check if both are different, if yes, return the difference */
          if (ia1[i] != ia2[i])
               /* return the difference */
               return ia1[i] - ia2[i];
     /* check if we reached the end of only one of the two versions */
     if ( ia1[i] != ia2[i] )
          /* return the difference */
          return ia1[i] - ia2[i];
     /* check if the two version characters differ */
     if (atom1->verc != atom2->verc)
          /* return the difference */
          return atom1->verc - atom2->verc;
     /* return the difference between the two suffixes */
     return atom1->sufenum - atom2->sufenum;
}
