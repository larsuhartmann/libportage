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
 * @brief parses a suffix string.
 *
 * @param suffix a @c nul terminated C string with the suffix.
 *
 * @return the suffix number.
 */
static inline lpversion_sufenum_t
lpversion_suffix_parse(const char *suffix);

extern lpversion_t *
lpversion_create(void)
{
     return malloc(sizeof(lpversion_t));
}

extern void
lpversion_init(lpversion_t *handle)
/*@sets handle@*//*@ensures isnull handle->version, handle->va@*/
{
     handle->version = NULL;
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
     free(handle->version);
     handle->version = NULL;
     handle->suffv = 0;
     handle->release = 0;
     handle->verc = (char)0;
     
     return;
}

extern void
lpversion_destroy(lpversion_t *handle)
{
     free(handle->version);
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
     handle->version = strdup(version);
     
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

#  ifdef __cplusplus
}
#  endif
