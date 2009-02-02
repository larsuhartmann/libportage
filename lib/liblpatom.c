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

extern lpatom_t *
lpatom_parse(lpatom_t *atom, const char *pname)
{
     regex_t regexp[1];
     regmatch_t regmatch[512];
     char *relv, *suf, *sufv, *ver, *vers, *vert;
     size_t len = 0;
     bool has_cat = false;

     /* initialize atom struct */
     if ( (atom = lpatom_create()) == NULL )
          return NULL;
     lpatom_init(atom);

     /* check if this is a valid atom */
     if ( regexec(&atom->regex.name, pname, 0, regmatch, 0) != 0) {
          lpatom_destroy(atom);
          errno = EINVAL;
          return NULL;
     }

     /* check if this atom has a category */
     if ( regexec(&atom->regex.category, pname, 2, regmatch, 0) == 0) {
          /* assign the category string to atom->cat */
          if ( (atom->cat = lputil_get_re_match(regmatch, 1, pname)) == NULL ) {
               lpatom_destroy(atom);
               return NULL;
          }
          has_cat = true;
     }

     regexec(&atom->regex.name, pname, 3, regmatch, 0);
     /* assign the package name to atom->name */
     if ( (atom->name = lputil_get_re_match(regmatch, 2, pname)) == NULL ) {
          lpatom_destroy(atom);
          return NULL;
     }

     /* count the length of the part before the version */
     if ( has_cat )
          len += (strlen(atom->cat) + 1);
     len += strlen(atom->name);

     /* snip off the rest of the string (shoud include the version plus the
      * suffix and release version */
     if ( (vers = strdup(pname+len+1)) == NULL ) {
          lpatom_destroy(atom);
          return NULL;
     }

     regexec(&atom->regex.version, vers, 1, regmatch, 0);

     /* get the version as string */
     if ( (ver = lputil_get_re_match(regmatch, 0, vers)) == NULL ) {
          free(vers);
          lpatom_destroy(atom);
          return NULL;
     }
     
     /* get the length of ver */
     len = strlen(ver);
     
     /* check if the digit of the version is a lowercase char, if yes, assign
      * that to atom->verc and resize ver */
     if ( islower(ver[len-1]) ) {
          atom->verc = ver[len-1];
          ver[len-1] = '\0';
          /* resize ver */
          if ( (vert = realloc(ver, len)) == NULL ) {
               free(vers);
               free(ver);
               lpatom_destroy(atom);
               return NULL;
          }
          ver = vert;
     }
     
     /* assign ver to atom->ver */
     atom->ver = ver;
     
     /* parse the version into a string array */
     if ( (atom->ver_ex = lpatom_version_explode(atom->ver)) == NULL ) {
          free(vers);
          lpatom_destroy(atom);
          return NULL;
     }

     /* check if it matches */
     if ( regexec(&atom->regex.suffix, vers, 3, regmatch, 0) == 0) {
          /* assign the first match (the suffix string) to suf  */
          if ( (suf = lputil_get_re_match(regmatch, 1, vers)) == NULL ) {
               free(vers);
               lpatom_destroy(atom);
               return NULL;
          }
          /* assign the second match (the suffix version) to sufv */
          if ( (sufv = lputil_get_re_match(regmatch, 2, vers)) == NULL ) {
               free(suf);
               free(vers);
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

     
     /* check if the regex matches */
     if ( regexec(&atom->regex.release, vers, 2, regmatch, 0) == 0 ) {
          /* assign the matched substring to relv */
          if ( (relv = lputil_get_re_match(regmatch, 1, vers)) == NULL ) {
               free(vers);
               lpatom_destroy(atom);
               return NULL;
          }
          /* convert the substring to int and assign it to atom->rel */
          atom->rel = atoi(relv);
          free(relv);
     }
     /* free up the regexp object */
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
               break;
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

extern lpatom_t *
lpatom_create(void)
{
     /* return newly allocated memory */
     return malloc(sizeof(lpatom_t));
}

extern void
lpatom_init(lpatom_t *atom)
{
     /* initialize struct */
     atom->name = NULL;
     atom->cat = NULL;
     atom->ver = NULL;
     atom->ver_ex = NULL;
     atom->verc = 0;
     atom->sufenum = no;
     atom->sufv = 0;
     atom->rel = 0;

     /* compile regexp's */
     regcomp(&atom->regex.atom, LPATOM_RE, REG_EXTENDED);
     regcomp(&atom->regex.category, LPATOM_RE_CAT, REG_EXTENDED);
     regcomp(&atom->regex.name, LPATOM_RE_NAME, REG_EXTENDED);
     regcomp(&atom->regex.suffix, LPATOM_RE_SUF, REG_EXTENDED);
     regcomp(&atom->regex.release, LPATOM_RE_REL, REG_EXTENDED);
     regcomp(&atom->regex.version, LPATOM_RE_VER, REG_EXTENDED);

     return;
}

extern void
lpatom_reinit(lpatom_t *atom) {
     /* free up pointers */
     free(atom->name);
     free(atom->cat);
     free(atom->ver);
     free(atom->ver_ex);

     /* initialize struct */
     atom->name = NULL;
     atom->cat = NULL;
     atom->ver = NULL;
     atom->ver_ex = NULL;
     atom->verc = 0;
     atom->sufenum = no;
     atom->sufv = 0;
     atom->rel = 0;

     return;
}

extern void
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
     char *suf, *r;
     size_t len;
     /* check if we have a suffix */
     if ( atom->sufenum != no ) {
          /* get suffix string */
          if ( (suf = lpatom_suffe_to_string(atom->sufenum)) == NULL )
               return NULL;
          /* check if we got a suffix version number */
          if ( atom->sufv != 0 ) {
               len = strlen(suf) + lputil_intlen(atom->sufv) + 1;
               /* allocate memory */
               if ( (r = malloc(len)) == NULL ) {
                    free(suf);
                    return NULL;
               }
               /* r = suf + 'atom->sufv' */
               snprintf(r, len, "%s%d", suf, atom->sufv);
               free(suf);
          } else {
               /* r = suf */
               r = suf;
               suf = NULL;
          }
     } else
          /* r = "" */
          if ( (r = strdup("")) == NULL)
               return NULL;
     return r;
}

static char *
lpatom_get_release(const lpatom_t *atom)
{
     char *r;
     size_t rellen;
     /* check if we have a release number */
     if (atom->rel > 0) {
          rellen = lputil_intlen(atom->rel)+2;
          /* allocate memory */
          if ( (r = malloc(rellen)) == NULL )
               return NULL;
          /* r = 'atom->rel' */
          snprintf(r, rellen, "r%d", atom->rel);
     } else
          /* r = "" */
          if ( (r = strdup("")) == NULL )
               return NULL;
     return r;
}

extern char *
lpatom_get_qname(const lpatom_t *atom)
{
     char *r;
     size_t len;

     /* check if we have a category */
     if ( atom->cat != NULL ) {
          len = strlen(atom->cat) + strlen(atom->name) + 2;
          /* allocate memory */
          if ( (r = malloc(len)) == NULL )
               return NULL;
          /* r = atom->cat + '/' + atom->name */
          snprintf(r, len, "%s/%s", atom->cat, atom->name);
     } else
          /* r = atom->name */
          if ( (r = strdup(atom->name)) == NULL )
               return NULL;
     return r;
}

static char *
lpatom_suffe_to_string(lpatom_sufe_t suffix)
{
     char *suf;
     
     switch(suffix){
     case alpha:
          if ( (suf = strdup("alpha")) == NULL )
               return NULL;
          break;
     case beta:
          if ( (suf = strdup("beta")) == NULL )
               return NULL;
          break;
     case pre:
          if ( (suf = strdup("pre")) == NULL )
               return NULL;
          break;
     case rc:
          if ( (suf = strdup("rc")) == NULL )
               return NULL;
          break;
     case p:
          if ( (suf = strdup("p")) == NULL )
               return NULL;
          break;
     default:
          if ( (suf = strdup("")) == NULL )
               return NULL;
          break;
     }
     return suf;
}

extern char *
lpatom_get_version(const lpatom_t *atom)
{
     size_t suflen, rellen, verlen, len;
     char *r, *suf, *sufs, *rel, *rels, *vers;

     /* get suffix string */
     if ( (suf = lpatom_get_suffix(atom)) == NULL )
          return NULL;
     /* get release string */
     if ( (rel = lpatom_get_release(atom)) == NULL ) {
          free(suf);
          return NULL;
     }

     /* check if we got a suffix */
     if ( strcmp(suf, "") != 0 ) {
          suflen = strlen(suf) + 1;
          /* allocate memory */
          if ( (sufs = malloc(suflen + 1)) == NULL ) {
               free(suf);
               free(rel);
               return NULL;
          }
          /* sufs = '_' + suffix */
          snprintf(sufs, suflen + 1, "_%s", suf);
          free(suf);
     } else {
          /* sufs = "" */
          sufs = suf;
          suf = NULL;
          suflen = 0;
     }

     /* check if we got a release */
     if ( strcmp(rel, "") != 0 ) {
          rellen = strlen(rel) + 1;
          /* allocate memory */
          if ( (rels = malloc(rellen + 1)) == NULL ) {
               free(sufs);
               free(rel);
               return NULL;
          }
          /* rels = '-' + rel */
          snprintf(rels, rellen + 1, "-%s", rel);
          free(rel);
     } else {
          /* rels = "" */
          rels = rel;
          rel = NULL;
          rellen = 0;
     }

     /* check if we got a version character */
     if ( atom->verc != 0 ) {
          verlen = strlen(atom->ver) + 1;
          /* allocate memory */
          if ( (vers = malloc(verlen + 1)) == NULL ) {
               free(sufs);
               free(rels);
               return NULL;
          }
          /* vers = atom->ver + 'atom->verc' */
          snprintf(vers, verlen + 1, "%s%c", atom->ver, atom->verc);
     } else {
          /* vers = atom->ver */
          if ( (vers = strdup(atom->ver)) == NULL )
               return NULL;
          verlen = strlen(vers);
     }

     len = suflen + rellen + verlen + 1;
     /* allocate memory for the whole version string */
     if ( (r = malloc(len)) == NULL ) {
          free(sufs);
          free(rels);
          free(vers);
          return NULL;
     }
     /* r = vers + sufs + rels */
     snprintf(r, len, "%s%s%s", vers, sufs, rels);
     
     /* clean up memory */
     free(vers);
     free(sufs);
     free(rels);
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
     for ( i=0; sv[i] != NULL; ++i )
          ;
     /* allocate space for an int array with i entrys */
     if ( (ia = malloc(sizeof(int)*(i+1))) == NULL ) {
          /* if allocation wasnt successful, throw away sv and return NULL *
           * (errno is already set by malloc) */
          lputil_splitstr_destroy(sv);
          return NULL;
     }
     /* iterate over the string array which was returned by
      * lputil_splitstr() */
     for ( i=0; sv[i] != NULL; ++i )
          /* convert the current string to an int and assign that one to
           * ia[i] */
          ia[i] = atoi(sv[i]);
     /* clean up the string array which was returned by lputil_splitstr() */
     ia[i] = -1;
    lputil_splitstr_destroy(sv);
     return ia;
}


extern int
lpatom_version_cmp(const lpatom_t *atom1, const lpatom_t *atom2)
{
     int *ia1, *ia2, i;

     ia1 = atom1->ver_ex;
     ia2 = atom2->ver_ex;
     
     /* iterate over both int arrays until the end of one of em is reached */
     for ( i=0; ia1[i] != -1 || ia2[i] != -1; ++i )
          /* check if both are different, if yes, return the difference */
          if ( ia1[i] != ia2[i] )
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

extern int
lpatom_cmp(const lpatom_t *atom1, const lpatom_t *atom2)
{
     int ret;

     /* check if both atoms have a category string */
     if ( atom1->cat != NULL && atom2->cat != NULL )
          /* check if the category string differs */
          if ( (ret = strcmp(atom1->cat, atom2->cat)) != 0 )
               return ret;
     /* check if the atom names differ */
     if ( (ret = strcmp(atom1->name, atom2->name)) != 0 )
          return ret;
     /* compare versions */
     return lpatom_version_cmp(atom1, atom2);
}

extern char *
lpatom_get_fullname(const lpatom_t *atom)
{
     char *qname, *ver, *r;
     size_t len = 0;

     /* get qname and version string */
     if ( (qname = lpatom_get_qname(atom)) == NULL )
          return NULL;
     if ( (ver = lpatom_get_version(atom)) == NULL ) {
          free(qname);
          return NULL;
     }

     /* add length of qname and version string, plus one byte for the '-' and
      * one for the nul terminator */
     len = strlen(qname) + 1;
     len += strlen(ver);
     ++len;

     /* allocate memory */
     if ( (r = malloc(len)) == NULL ) {
          free(qname);
          free(ver);
          return NULL;
     }

     /* r = qname+'-'+ver */
     snprintf(r, len, "%s-%s", qname, ver); 

     /* free up the qname and ver strings */
     free(qname);
     free(ver);

     return r;
}
