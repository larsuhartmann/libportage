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
#include <ctype.h>

/**
 * \brief Regular expression for a package atom.
 */
#define LPATOM_RE         "^(([A-Za-z0-9+_][A-Za-z0-9+_.-]*)?/?([A-Za-z0-9+_-]\
*[A-Za-z+_-]|[A-Za-z+_][0-9]+))-([0-9]+([.][0-9]+)*[a-z]?)((_(alpha|beta|pre|\
rc|p)[0-9]*)?(-r[0-9]+)?)?$"
/**
 * \brief Regular expression for a category
 */
#define LPATOM_RE_CAT     "^([A-Za-z0-9+_][A-Za-z0-9+_.-]*)/"
/**
 * \brief Regular expression for a package name
 */
#define LPATOM_RE_NAME    "^([A-Za-z0-9+_][A-Za-z0-9+_.-]*/)?([A-Za-z0-9+_-]*\
([A-Za-z+_-]|[A-Za-z+_][0-9]+))-"
/**
 * \brief Regular expression to check for a suffix version
 */
#define LPATOM_RE_SUF    "_(alpha|beta|pre|rc|p)([0-9]+)"
/**
 * \brief Regular expression for a package release version
 */
#define LPATOM_RE_REL     "-r([0-9]+)$"
/**
 * \brief Regular expression for a Package version
 */
#define LPATOM_RE_VER     "^([0-9]+([.][0-9]+)*)([a-z]?)"

/**
 * Reads the suffix out of an suffix string
 * 
 * gets an suffix string (eg alpha) and returns an lpatom_suffe_t enum.
 *
 * \param s a \c nul terminated c string with the suffix
 *
 * \return an lpatom_suffe_t enum.
 */
static lpatom_sufe_t
lpatom_suffix_parse(const char *s);

/**
 * allocates and initializes a new lpatom_t memory structure.
 *
 * Allocates enough space for a lpatom_t memory structure on the heap and sets
 * all of its pointers or integers to \c NULL or \c 0.
 *
 * The memory for the returned struct is allocated by malloc and is not used
 * elsewhere in this lib, if you want to destroy it, use lpatom_destroy().
 * 
 * If an error occured, \c Null is returned and \c errno is set to indicate the
 * error.
 *
 * \return a pointer to an newly initialized lpatom_t memory structure or \c
 * NULL if an error has occured.
 *
 * \b Errors:
 *
 * - This function may fail and set errno for any of the errors specified for
 *   the routine malloc(3).
 *
 * \sa lpatom_destroy().
 */
static lpatom_t *
lpatom_init(void);

/**
 * \brief parses a atom_suffe_t enum into a string.
 * 
 * Returns the Suffix as a \c nul terminated c String.
 *
 * If an error occured, \c NULL is returned and \c errno is set to indicate
 * the Error.
 *
 * \param suffix a lpatom_sufe_t enum.
 *
 * \return a \c nul terminated c string with the suffix.
 *
 * \b Errors:
 *
 * - This function may also fail and set errno for any of the errors specified
 *   for the routine strdup(3).
 */
static char *
lpatom_suffe_to_string(lpatom_sufe_t suffix);

/**
 * \brief parses an version string.
 *
 * Parses an version string into multiple integers and returns a \c -1
 * terminated int array.
 *
 * If an error occured, \c NULL is returned and \c errno is set to indicate
 * the error.
 *
 * \param ver a \c nul terminated version string eg: 4.2.1.9
 *
 * \return a \c -1 terminated integer array.
 *
 * \b Errors:
 *
 * - This function may fail and set errno for any of the errors specified for
 *   the routine strdup(3).
 * - This function may also fail and set errno for any of the errors specified
 *   for the routine malloc(3).
 * - This function may also fail and set errno for any of the errors specified
 *   for the routine realloc(3).
 */
static int *
lpatom_version_explode(const char *ver);

/**
 * \brief returns the suffix of an atom as a c string.
 *
 * Returns a String containing the version suffix of a given lpatom_t data
 * structure.
 *
 * If the atom has no suffix, an empty string ("") is returned.
 *
 * If an error occured, \c NULL is returned and \c errno is set to indicate
 * the error.
 *
 * \param atom a pointer to an lpatom_t data structure that was returned by
 * lpatom_parse().
 *
 * \return a string containing the suffix, an empty string if the atom has no
 * suffix or \c NULL if an error has occured.
 *
 * \b Errors:
 *
 * - This function may also fail and set errno for any of the errors specified
 *   for the routine malloc(3).
 */
static char *
lpatom_get_suffix(const lpatom_t *atom);

/**
 * \brief returns the release of a given atom.
 *
 * Returns a \c nul terminated string with the release of the given atom - eg
 * \c "r29".
 *
 * If an error occured, \c NULL is returned and \c errno is set to indicate
 * the error.
 *
 * \param atom a pointer to an lpatom_t data structure that was returned by
 * lpatom_parse().
 *
 * \return a string containing the release version, an empty string if the
 * atom has no release version or \c NULL if an error has occured.
 *
 * \b Errors:
 *
 * - This function may fail and set errno for any of the errors specified for
 *   the routine strdup(3).
 * - This function may also fail and set errno for any of the errors specified
 *   for the routine malloc(3).
 */

static char *
lpatom_get_release(const lpatom_t *atom);

lpatom_t *
lpatom_parse(const char *pname)
{
     regex_t regexp[1];
     regmatch_t regmatch[512];
     lpatom_t *atom = NULL;
     char *relv, *suf, *sufv, *ver, *vers, *t;
     size_t len = 0;
     bool has_cat = false;

     /* initialize atom struct */
     if ( (atom = lpatom_init()) == NULL)
          return NULL;

     /* compile regexp */
     regcomp(regexp, LPATOM_RE, REG_EXTENDED);
     /* check if this is a valid atom */
     if ( regexec(regexp, pname, 0, regmatch, 0) != 0) {
          regfree(regexp);
          lpatom_destroy(atom);
          errno = EINVAL;
          return NULL;
     }
     /* free up the compiled regexp */
     regfree(regexp);

     /* compile the category regexp */
     regcomp(regexp, LPATOM_RE_CAT, REG_EXTENDED);
     /* check if this atom has a category */
     if ( regexec(regexp, pname, 2, regmatch, 0) == 0) {
          /* assign the category string to atom->cat */
          if ( (atom->cat = lputil_get_re_match(regmatch, 1, pname)) == NULL ) {
               regfree(regexp);
               lpatom_destroy(atom);
               return NULL;
          }
          has_cat = true;
     }
     /* free up the compiled regexp */
     regfree(regexp);

     /* compile the package name regexp */
     regcomp(regexp, LPATOM_RE_NAME, REG_EXTENDED);
     regexec(regexp, pname, 3, regmatch, 0);
     /* assign the package name to atom->name */
     if ( (atom->name = lputil_get_re_match(regmatch, 2, pname)) == NULL ) {
          regfree(regexp);
          lpatom_destroy(atom);
          return NULL;
     }
     /* free up the compiled regexp */
     regfree(regexp);

     /* count the length of the part before the version */
     if ( has_cat )
          len += (strlen(atom->cat) + 1);
     len += strlen(atom->name);

     /* snip off the rest of the string (shoud include the version plus the
      * suffix and release version */
     if ( (vers = strdup(pname+len+1)) == NULL) {
          lpatom_destroy(atom);
          return NULL;
     }

     /* compile the version regexp */
     regcomp(regexp, LPATOM_RE_VER, REG_EXTENDED);
     regexec(regexp, vers, 1, regmatch, 0);

     /* get the version as string */
     if ( (ver = lputil_get_re_match(regmatch, 0, vers)) == NULL ) {
          free(vers);
          regfree(regexp);
          lpatom_destroy(atom);
          return NULL;
     }
     /* free up the regexp object */
     regfree(regexp);
     
     /* get the length of ver */
     len = strlen(ver);
     
     /* check if the digit of the version is a lowercase char, if yes, assign
      * that to atom->verc and resize ver */
     if ( islower(ver[len-1]) ) {
          atom->verc = ver[len-1];
          ver[len-1] = '\0';
          /* resize ver */
          if ( (t = realloc(ver, len)) == NULL) {
               free(vers);
               free(ver);
               regfree(regexp);
               lpatom_destroy(atom);
               return NULL;
          }
          ver = t;
     }
     
     /* assign ver to atom->ver */
     atom->ver = ver;
     
     /* parse the version into a string array */
     if ( (atom->ver_ex = lpatom_version_explode(atom->ver)) == NULL ) {
          free(vers);
          lpatom_destroy(atom);
          return NULL;
     }

     /* compile the suffix regexp */
     regcomp(regexp, LPATOM_RE_SUF, REG_EXTENDED);
     /* check if it matches */
     if ( regexec(regexp, vers, 3, regmatch, 0) == 0) {
          /* assign the first match (the suffix string) to suf  */
          if ( (suf = lputil_get_re_match(regmatch, 1, vers)) == NULL) {
               free(vers);
               regfree(regexp);
               lpatom_destroy(atom);
               return NULL;
          }
          /* assign the second match (the suffix version) to sufv */
          if ( (sufv = lputil_get_re_match(regmatch, 2, vers)) == NULL) {
               free(suf);
               free(vers);
               regfree(regexp);
               lpatom_destroy(atom);
               return NULL;
          }
          /* assign the parsed suffix string to atom->sufenum */
          atom->sufenum = lpatom_suffix_parse(suf);
          /* assign the converted sufv string to atom->sufv */
          atom->sufv = atoi(sufv);

          /* free up the allocated memory */
          free(suf);
          free(sufv);
     }
     regfree(regexp);

     
     /* compile the release version regexp */
     regcomp(regexp, LPATOM_RE_REL, REG_EXTENDED);

     /* check if the regex matches */
     if ( regexec(regexp, vers, 2, regmatch, 0) == 0) {
          /* assign the matched substring to relv */
          if ( (relv = lputil_get_re_match(regmatch, 1, vers)) == NULL) {
               free(vers);
               regfree(regexp);
               lpatom_destroy(atom);
               return NULL;
          }
          /* convert the substring to int and assign it to atom->rel */
          atom->rel = atoi(relv);
          free(relv);
     }
     /* free up the regexp object */
     regfree(regexp);
     free(vers);
     return atom;
}

static lpatom_sufe_t
lpatom_suffix_parse(const char *s)
{
     lpatom_sufe_t suf;
     
     switch(s[0]) {
     case 'a':
          suf = alpha;
          break;
     case 'b':
          suf = beta;
          break;
     case 'p':
          switch(s[1]) {
          case 'r':
               suf = pre;
               break;
          default:
               suf = p;
          }
          break;
     case 'r':
          suf = rc;
          break;
     default:
          suf = no;
          break;
     }
     return suf;
}


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

static char *
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

static char *
lpatom_get_release(const lpatom_t *atom)
{
     char *r;
     char *t;
     if (atom->rel > 0) {
          if ( (t = malloc(lputil_intlen(atom->rel)+1)) == NULL )
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
          if ( (cat = malloc(len+2)) == NULL )
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
     if ( (r = malloc(len)) == NULL )
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
     size_t len, suflen, rellen;
     char *r;
     char *suf;
     char *rel;
     char *verc = NULL;
     
     len = strlen(atom->ver);
     suf = lpatom_get_suffix(atom);
     suflen = strlen(suf);
     if ( suflen > 0 )
          len += suflen + 1;
     rel = lpatom_get_release(atom);
     rellen = strlen(rel);
     if ( rellen > 0 )
          len += rellen + 1;
     if (atom->verc > 0) {
          if ( (verc = malloc(1)) == NULL) {
               free(suf);
               free(rel);
               return NULL;
          }
          sprintf(verc, "%c", atom->verc);
          ++len;
     }
     if ( (r = malloc(len)) == NULL)
          return NULL;
     strcpy(r, atom->ver);
     if ( atom->verc > 0) {
          sprintf(verc, "%c", atom->verc);
          strcat(r, verc);
     }
     if ( suflen > 0) {
          strcat(r, "_");
          strcat(r, suf);
     }
     if (rellen > 1) {
          strcat(r, "-");
          strcat(r, rel);
     }
     free(rel);
     free(verc);
     free(suf);
     return r;
}

static int *
lpatom_version_explode(const char *ver) {
     int *ia;
     size_t i;
     char **sv;

     /* split up the version string */
     if ( (sv = lputil_splitstr(ver, ".")) == NULL )
          /* return NULL if error was received */
          return NULL;
     /* get the number of individual strings */
     for (i=0; sv[i] != NULL; ++i)
          ;
     /* allocate space for an int array with i entrys */
     if ( (ia = malloc(sizeof(int)*(i+1))) == NULL) {
          /* if allocation wasnt successful, throw away sv and return NULL *
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
     if ( atom1->verc != atom2->verc )
          /* return the difference */
          return atom1->verc - atom2->verc;
     /* check if the two suffixes differ */
     if ( atom1->sufenum != atom2->sufenum )
          /* return the difference */
          return atom1->sufenum - atom2->sufenum;
     /* return the difference between the release versions */
     return atom1->rel - atom2->rel;
}

int
lpatom_cmp(const lpatom_t *atom1, const lpatom_t *atom2)
{
     int ret;
     if ( (ret = strcmp(lpatom_get_qname(atom1), lpatom_get_qname(atom2)))
         != 0 )
          return ret;
     return lpatom_version_cmp(atom1, atom2);
}

char *
lpatom_get_fullname(const lpatom_t *atom)
{
     char *qname;
     char *ver;
     size_t len = 0;
     char *r;

     qname = lpatom_get_qname(atom);
     len = strlen(qname) + 1;
     ver = lpatom_get_version(atom);
     len += strlen(ver);

     if ( (r = malloc(len)) == NULL) {
          free(qname);
          free(ver);
          return NULL;
     }

     strcpy(r, qname);
     strcat(r, "-");
     strcat(r, ver);

     return r;
}
