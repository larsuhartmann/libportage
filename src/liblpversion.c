/*
 * Copyright (c) 2009 Lars Hartmann
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

/** @brief Feature test macro for POSIX.1, POSIX.2, XPG4, SUSv2, SUSv3. */
#define _XOPEN_SOURCE   600

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <version.h>
#include <util.h>

#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <inttypes.h>
#include <stdbool.h>

#if HAVE_ERRNO_H
#  include <errno.h>
#endif /* HAVE_ERRNO_H */
#ifndef errno
/* Some Systems #define this! */
extern int errno;
#endif /*! errno */

#  ifdef __cplusplus
extern "C" {
#  endif

/**
 * @brief the regular expression used for validating and tokenizing of the
 * given version string.
 *
 * This regexp will export:
 *
 * - The numeric version part with optional version character.
 */
#define LPVERSION_RE            "^([0-9]+([.][0-9]+)*[a-z]?)((_(alpha|beta|pre\
|rc|p)[0-9]*)?)(-r[0-9]+)?$"

/**
 * @brief regular expression for matching the suffix and the suffix version
 *
 * This regular expression will export:
 *
 * - The suffix string (alpha, beta ..)
 * - The suffix Version
 */
#define LPVERSION_RE_SUF        "_(alpha|beta|pre|rc|p)([0-9]+)"

/**
 * @brief regex for matching the release version
 *
 * This regular expression will export:
 *
 * - The release version number.
 */
#define LPVERSION_RE_REL        "-r([0-9]+)$"

/**
 * @brief parses the numerical part of a version string into an @c -1
 * terminated int array.
 *
 * If an error occurs, \c NULL is returned and \c errno is set to indicate
 * the error.
 *
 * @param ver a \c nul terminated version string eg: 4.2.1.9.
 *
 * @return a \c -1 terminated integer array.
 *
 * @b Errors:
 *
 * - This function may fail and set errno for any of the errors specified for
 *   the routine strdup(3).
 * - This function may also fail and set errno for any of the errors specified
 *   for the routine malloc(3).
 * - This function may also fail and set errno for any of the errors specified
 *   for the routine realloc(3).
 */
/*@null@*//*@only@*/
static inline int64_t *
lpversion_version_parse(const char *ver);

/**
 * @brief compiles the numerical version part.
 *
 * If an error occurs, \c NULL is returned and \c errno is set to indicate
 * the error.
 *
 * @param ver a \c nul terminated version string eg: 4.2.1.9.
 *
 * @return a \c -1 terminated integer array.
 *
 * @b Errors:
 *
 * - This function may fail and set errno for any of the errors specified for
 *   the routine strdup(3).
 * - This function may also fail and set errno for any of the errors specified
 *   for the routine malloc(3).
 * - This function may also fail and set errno for any of the errors specified
 *   for the routine realloc(3).
 */
/*@null@*//*@only@*/
static inline char *
lpversion_version_compile(const lpversion_t *handle);

/**
 * @brief parses a suffix string.
 *
 * @param suffix a @c nul terminated C string with the suffix.
 *
 * @return the suffix number.
 */
static inline lpversion_sufenum_t
lpversion_suffix_parse(const char *suffix);

/**
 * @brief compiles the suffix into a string.
 *
 * @param handle a lpversion_t object.
 *
 * @return a @c null terminated character string.
 */
static inline char *
lpversion_suffix_compile(const lpversion_t *handle);

extern lpversion_t *
lpversion_create(void)
{
     return malloc(sizeof(lpversion_t));
}

extern void
lpversion_init(lpversion_t *handle)
/*@sets handle@*//*@ensures isnull handle->version, handle->va@*/
{
     handle->va = NULL;
     handle->suffv = 0;
     handle->release = 0;
     handle->verc = (char)0;
     handle->suffix = LPV_NO;
     (void)regcomp(&handle->regex.verify, LPVERSION_RE, REG_EXTENDED);
     (void)regcomp(&handle->regex.suffix, LPVERSION_RE_SUF, REG_EXTENDED);
     (void)regcomp(&handle->regex.release, LPVERSION_RE_REL, REG_EXTENDED);

     return;
}

extern void
lpversion_reset(lpversion_t *handle)
{
     free(handle->va);
     handle->va = NULL;
     handle->suffix = LPV_NO;
     handle->suffv = 0;
     handle->suffix = LPV_NO;
     handle->release = 0;
     handle->verc = (char)0;

     return;
}

extern void
lpversion_destroy(lpversion_t *handle)
{
     free(handle->va);
     regfree(&handle->regex.verify);
     regfree(&handle->regex.suffix);
     regfree(&handle->regex.release);
     free(handle);

     return;
}

extern int
lpversion_parse(lpversion_t *handle, const char *version)
/*@requires isnull handle->va, handle->version@*/
{
     regmatch_t match[3];
     char *tmp;
     size_t len;

     /* validate input */
     if ( regexec(&handle->regex.verify, version, 3, match, 0) != 0 ) {
          errno = EINVAL;
          return -1;
     }
     if ( (tmp = lputil_get_re_match(match, 1, version)) == NULL )
          return -1;
     len = strlen(tmp);
     if ( islower((int)tmp[len-1]) != 0 ) {
          handle->verc = tmp[len-1];
          tmp[len-1] = '\0';
     }
     if ( (handle->va = lpversion_version_parse(tmp)) == NULL ) {
          free(tmp);
          return -1;
     }
     free(tmp);
     if ( regexec(&handle->regex.suffix, version, 3, match, 0) == 0 ) {
          if ( (tmp = lputil_get_re_match(match, 1, version)) == NULL )
               return -1;
          handle->suffix = lpversion_suffix_parse(tmp);
          free(tmp);
          if ( (tmp = lputil_get_re_match(match, 2, version)) == NULL )
               return -1;
          handle->suffv = (unsigned int)atoi(tmp);
          free(tmp);
     }
     if ( regexec(&handle->regex.release, version, 3, match, 0) == 0 ) {
          if ( (tmp = lputil_get_re_match(match, 1, version)) == NULL )
               return -1;
          handle->release = (unsigned int)atoi(tmp);
          free(tmp);
     }

     return 0;
}

extern int
lpversion_cmp(const lpversion_t *v1, const lpversion_t *v2)
/*@requires notnull v1->va, v2->va@*/
{
     int i;

     for ( i=0; v1->va[i] != -1 || v2->va[i] != -1; ++i )
          if ( v1->va[i] != v2->va[i] )
               return (int)(v1->va[i] - v2->va[i]);

     if ( v1->va[i] != v2->va[i] )
          return (int)(v1->va[i] - v2->va[i]);

     if ( v1->verc != v2->verc )
          return (int)(v1->verc - v2->verc);

     if ( v1->suffix != v2->suffix )
          return (int)(v1->suffix - v2->suffix);

     return (int)(v1->release - v2->release);
}

static inline int64_t *
lpversion_version_parse(const char *ver)
{
     int64_t *ia;
     size_t len, i;
     char **sv;

     if ( (sv = lputil_splitstr(ver, ".")) == NULL )
          return NULL;
     for ( i=0; sv[i] != NULL; ++i )
          ;
     len = i++;
     if ( (ia = malloc(sizeof(int64_t)*i)) == NULL ) {
          lputil_splitstr_destroy(sv);
          return NULL;
     }
     for ( i=0; i < len; ++i )
          /* see inttypes.h for SCNi64 */
          (void)sscanf(sv[i], "%"SCNi64, &ia[i]);
     ia[len] = -1;
     lputil_splitstr_destroy(sv);

     return ia;
}

/*@null@*//*@only@*/
static inline char *
lpversion_version_compile(const lpversion_t *handle)
{
     unsigned int i, j;
     size_t len;
     char **va, *ret, *tmp, *tmp2;

     for ( i=0; handle->va[i] != -1; ++i )
          ;
     if ( (va = malloc(sizeof(char *)*(i+2))) == NULL )
          return NULL;

     for ( j=0; j<i; ++j ) {
          if ( (va[j] = malloc(lputil_int64len(handle->va[j])+1)) == NULL ) {
               for ( i=0; i<j; ++i )
                    free(va[i]);
               return NULL;
          }
          (void)sprintf(va[j], "%"PRIi64, handle->va[j]);
     }
     va[j] = NULL;
     ret = lputil_joinstr((const char **)va, ".");
     for (j=0; j<i; ++j)
          free(va[j]);
     free(va);

     if (handle->verc != 0 ) {
          if ( (tmp = malloc(strlen(ret)+2)) == NULL ) {
               free(ret);
               return NULL;
          }
          tmp2 = stpcpy(tmp, ret);
          free(ret);
          *tmp2++ = handle->verc;
          *tmp2 = '\0';
          ret = tmp;
     }
     return ret;
}

static inline lpversion_sufenum_t
lpversion_suffix_parse(const char *suffix)
{
     switch(suffix[0]) {
     case 'a':
          return LPV_ALPHA;
     case 'b':
          return LPV_BETA;
     case 'p':
          switch(suffix[1]) {
          case 'r':
               return LPV_PRE;
          default:
               return LPV_P;
          }
     case 'r':
          return LPV_RC;
     default:
          return LPV_NO;
     }
}

static inline char *
lpversion_suffix_compile(const lpversion_t *handle)
{
     char *suffix, *ret, *tmp;
     size_t len;

     switch(handle->suffix) {
     case LPV_ALPHA:
          suffix = strdup("alpha");
          break;
     case LPV_BETA:
          suffix = strdup("beta");
          break;
     case LPV_PRE:
          suffix = strdup("pre");
          break;
     case LPV_P:
          suffix = strdup("p");
          break;
     case LPV_RC:
          suffix = strdup("rc");
          break;
     default:
          suffix = strdup("");
          break;
     }

     len = strlen(suffix)+lputil_intlen(handle->suffv)+1;
     if ( (ret = malloc(len)) == NULL ) {
          free(suffix);
          return NULL;
     }

     tmp = stpcpy(ret, suffix);
     (void)sprintf(tmp, "%d", handle->suffv);
     free(suffix);
     return ret;
}

extern char *
lpversion_compile(const lpversion_t *handle)
{
     char *suffix=NULL, *numver, *ret, *tmp;
     size_t len = 0;

     if ( (numver = lpversion_version_compile(handle)) == NULL )
          return NULL;
     len += strlen(numver);
     if ( handle->suffix != LPV_NO ) {
          if ( (suffix = lpversion_suffix_compile(handle)) == NULL ) {
               free(numver);
               return NULL;
          }
          if ( suffix[0] != '\0') {
               len += strlen(suffix);
               len += lputil_intlen(handle->suffv);
               ++len;
          }
     }
     if ( handle->release > 0 ) {
          len += lputil_intlen(handle->release);
          len+=2;
     }
     if ( (ret = malloc(len+1)) == NULL ) {
          free(numver);
          free(suffix);
     }
     tmp = stpcpy(ret, numver);
     free(numver);
     if (suffix != NULL) {
          tmp = stpcpy(tmp, "_");
          tmp = stpcpy(tmp, suffix);
          free(suffix);
     }
     if (handle->release != 0) {
          tmp = stpcpy(tmp, "-r");
          (void)sprintf(ret+strlen(ret), "%d", handle->release);
     }

     return ret;
}

#  ifdef __cplusplus
}
#  endif
