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

/**
 * @brief Feature test macro for POSIX.1, POSIX.2, XPG4, SUSv2, SUSv3.
 *
 * This makes sure, we have a Standard conformant environment.
 */
#define _XOPEN_SOURCE   600
#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <atom.h>
#include <util.h>
#include <stdbool.h>

#include <sys/types.h>

#if HAVE_ERRNO_H
#  include <errno.h>
#endif /* HAVE_ERRNO_H */
#ifndef errno
/* Some Systems #define this! */
extern int errno;
#endif /*! errno */

#if STDC_HEADERS
#  include <stdlib.h>
#  include <string.h>
#elif HAVE_STRINGS_H
#  include <string.h>
#endif /* STDC_HEADERS */

#include <stdbool.h>
#include <ctype.h>

/* this function seems to be in glibc right from the beginning */
#ifdef __GNUC__
extern char *
stpcpy(char *dest, const char *src);
#endif

/**
 * @brief the regular expression used for validating the given version string.
 */
#define LPATOM_RE         "^(([A-Za-z0-9+_][A-Za-z0-9+_.-]*)/)?([A-Za-z0-9+_-]\
*([A-Za-z+_-]|[A-Za-z+_][0-9]+))(-(([0-9]+([.][0-9]+)*[a-z]?)?)((_(alpha|beta|\
pre|rc|p)[0-9]*)?)(-r[0-9]+)?)?$"

/**
 * @brief the regular expression used for matching the category-
 *
 * This regexp will export:
 *
 * - The category part.
 */
#define LPATOM_RE_CAT     "^([A-Za-z0-9+_][A-Za-z0-9+_.-]*)/"
/**
 * @brief the regular expression used for matching the package name
 *
 * This regexp will export:
 *
 * - The atom name.
 */
#define LPATOM_RE_NAME "([A-Za-z0-9+_-]*([A-Za-z+_-]|[A-Za-z+_][0-9]+))(-(([0-\
9]+([.][0-9]+)*[a-z]?)?)((_(alpha|beta|pre|rc|p)[0-9]*)?)(-r[0-9]+)?)?$"
/**
 * \@brief the regular expression used for matching the version.
 *
 * Thix regexp will export:
 *
 * - the version.
 */
#define LPATOM_RE_VER     "(([0-9]+([.][0-9]+)*[a-z]?)(_(alpha|beta|pre|rc|p)[\
0-9]*)?(-r[0-9]+)?)$"

#ifdef __cplusplus
extern "C" {
#endif

extern int
lpatom_parse(lpatom_t *handle, const char *s)
/*@requires isnull handle->fname, handle->cat, handle->name,
handle->version@*/
{
     regmatch_t match[512];
     char *vs;
     int ret;

     /* FIXME: add proper error handling */
     if ( regexec(&handle->regex.atom, s, 0, match, 0) != 0 ) {
          errno = EINVAL;
          return -1;
     }
     
     /* FIXME: add proper error handling */
     if ( regexec(&handle->regex.category, s, 4, match, 0 ) == 0 )
          if ( (handle->cat = lputil_get_re_match(match, 1, s)) == NULL )
               return -1;

     /* FIXME: add error handling */
     (void)regexec(&handle->regex.name, s, 5, match, 0 );
     if ( (handle->name = lputil_get_re_match(match, 1, s)) == NULL )
          return -1;

     if ( regexec(&handle->regex.version, s, 2, match, 0 ) == 0 ) {
          if ( (handle->version = lpversion_create()) == NULL )
               return -1;
          lpversion_init(handle->version);
          if ( (vs = lputil_get_re_match(match, 1, s)) == NULL )
               return -1;
          ret = lpversion_parse(handle->version, vs) == -1;
          free(vs);
          
          return ret;
     }

     return 0;
}

/*@only@*//*@null@*//*@out@*/
extern lpatom_t *
lpatom_create(void)
{
     return malloc(sizeof(lpatom_t));
}

extern void
lpatom_init(lpatom_t *handle)
/*@sets handle@*//*@ensures isnull handle->name, handle->cat,
handle->version, handle->fname@*/
{
     handle->name = NULL;
     handle->cat = NULL;
     handle->version = NULL;

     /* FIXME: add error handling */
     (void)regcomp(&handle->regex.atom, LPATOM_RE, REG_EXTENDED);
     (void)regcomp(&handle->regex.category, LPATOM_RE_CAT, REG_EXTENDED);
     (void)regcomp(&handle->regex.name, LPATOM_RE_NAME, REG_EXTENDED);
     (void)regcomp(&handle->regex.version, LPATOM_RE_VER, REG_EXTENDED);
     return;
}

extern void
lpatom_reset(lpatom_t *handle)
/*@sets handle@*//*@ensures isnull handle->name, handle->cat,
handle->version, handle->fname@*/
{
     free(handle->name);
     free(handle->cat);

     if ( handle->version != NULL ) {
          lpversion_destroy(handle->version);
          handle->version = NULL;
     }
     
     handle->name = NULL;
     handle->cat = NULL;
     handle->version = NULL;

     return;
}

extern void
lpatom_destroy(lpatom_t *handle)
{
     if (handle != NULL) {
          regfree(&handle->regex.atom);
          regfree(&handle->regex.category);
          regfree(&handle->regex.name);
          regfree(&handle->regex.version);

          if ( handle->version != NULL )
               lpversion_destroy(handle->version);
          
          free(handle->name);
          free(handle->cat);
          free(handle);
     }
     return;
}

extern int
lpatom_cmp(const lpatom_t *atom1, const lpatom_t *atom2)
{
     int ret;

     if ( atom1->cat != NULL && atom2->cat != NULL )
          if ( (ret = strcmp(atom1->cat, atom2->cat)) != 0 )
               return ret;
     
     if ( (ret = strcmp(atom1->name, atom2->name)) != 0 )
          return ret;
     
     if ( atom1->version != NULL && atom2->version != NULL )
          return lpversion_cmp(atom1->version, atom2->version);
     
     return 0;
}

extern char *
lpatom_compile(const lpatom_t *handle)
{
     char *ret, *ver = NULL, *tmp;
     size_t len = 1;

     if ( handle->version != NULL ) {
          if ( (ver = lpversion_compile(handle->version)) == NULL )
               return NULL;
          else {
               len += strlen(ver)+1;
          }
     }
     if ( handle->cat != NULL )
          len += strlen(handle->cat)+1;
     len += strlen(handle->name);

     if ( (ret = malloc(len)) == NULL ) {
          free(ver);
          return NULL;
     }
     tmp = ret;
     if ( handle->cat != NULL ) {
          tmp = stpcpy(tmp, handle->cat);
          tmp = stpcpy(tmp, "/");
     }
     tmp = stpcpy(tmp, handle->name);
     if ( ver != NULL ) {
          tmp = stpcpy(tmp, "-");
          tmp = stpcpy(tmp, ver);
          free(ver);
     }
     
     return ret;
}

#ifdef __cplusplus
}
#endif
