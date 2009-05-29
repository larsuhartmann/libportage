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
 * @file atom.h
 * @brief Functions to handle atom strings.
 */
#ifndef LPATOM
/** @cond */
#  define LPATOM 1
/** @endcond */

#  include <stdio.h>
#  include <regex.h>

#  include <version.h>

#  ifdef __cplusplus
extern "C" {
#  endif

/**
 * @brief struct for compiled regexes.
 */
typedef struct lpatom_regex{
     regex_t atom;     /**< @brief regex for matching a valid atom */
     regex_t category; /**< @brief regex to check for a category string */
     regex_t name;     /**< @brief regex to match the package name */
     regex_t version;  /**< @brief regex to match the package version */
} lpatom_regex_t;

/**
 * @brief The lpatom object handle.
 *
 * This is the handle to be used for any function in this lib to store some
 * date.
 *
 * @warning do not allocate or free it yourself, use lpatom_create() or
 * lpversion_destroy().
 */
typedef struct lpatom {
     lpatom_regex_t regex;      /**< @brief compiled regexes. */
     char *name;                /**< @brief the name. */
     char *cat;                 /**< @brief the category. */
     lpversion_t *version;      /**< @brief a version handle. */
} lpatom_t;

/**
 * @brief returns a new lpatom_t handle.
 *
 * The returned handle needs to be initialized using lpatom_init before using
 * it.
 * 
 * The memory for the returned struct is allocated by malloc and is not used
 * elsewhere in this lib, if you want to destroy it, use lpatom_destroy().
 *
 * If you used the handle for some kind of operation and want to reset it, use
 * lpatom_reset().
 * 
 * If an error occured, @c Null is returned and @c errno is set to indicate the
 * error.
 *
 * @return a pointer to a lpatom_t handle or @c NULL if an error has occured.
 *
 * @b Errors:
 *
 * - This function may fail and set errno for any of the errors specified for
 *   the routine malloc(3).
 *
 * @sa lpatom_init() lpatom_destroy().
 */
/*@only@*//*@null@*//*@out@*/
extern lpatom_t *
lpatom_create(void);

/**
 * @brief Initializes a lpatom_t handle.
 *
 * If you want to initialize an already used handle, use lpatom_reset()
 *
 * @param handle a lpatom_t handle to initialize.
 *
 * @sa lpatom_create(), lpatom_reset(), lpatom_destroy().
 */
extern void
lpatom_init(lpatom_t *handle);

/**
 * @brief resets an lpatom_t handle.
 *
 * @param handle a lpatom_t handle to initialize.
 *
 * @sa lpatom_create(), lpatom_init(), lpatom_destroy().
 */
extern void
lpatom_reset(lpatom_t *handle);

/**
 * @brief parses an atom string.
 *
 * The given string must be a valid gentoo atom with or without version.
 *
 * If an error occured, \c -1 is returned and \c errno is set to indicate the
 * error.
 *
 * @param handle a lpatom_t handle.
 * @param s a string with the full package version.
 *
 * @return @c 0 if sucessful or @c -1 if an error occured
 *
 * @sa lpatom_t, lpatom_create(), lpatom_init(), lpatom_destroy(),
 * lpatom_get_version(), lpatom_get_fullname(), lpatom_get_qname()
 *
 * @b Errors:
 * 
 * - @c EINVAL s is not a valid package atom.
 * - This function may also fail and set errno for any of the errors specified
 *   for the routine malloc(3).
 * - This function may also fail and set errno for any of the errors specified
 *   for the routine strdup(3). 
 */
extern int
lpatom_parse(lpatom_t *handle, const char *s);

/**
 * @brief destroys an lpatom_t object.
 *
 * frees up all corresponding memory regions using free(3). If a @c NULL
 * pointer was given, lpatom_destroy() will just return
 * 
 * @param atom a pointer to an lpxpak_t data structure that was returned by
 * lpatom_parse().
 *
 * @sa lpatom_t, lpatom_parse()
 *
 * @warning Do not try to access the datastructure after destroying it or
 * anything can happen.
 */
extern void
lpatom_destroy(lpatom_t *atom);

/**
 * @brief compare two lpatom_t data structures.
 *
 * Compares the two lpatom_t structures and returns an Integer less than,
 * equal to or greater than zero if atom1 is found, respectively, to be less
 * than, to match, or greater than atom2.
 *
 * This function can directly be used in qsort(3).
 *
 * @param atom1 the first lpatom_t struct which is going to be compared.
 *
 * @param atom2 the second lpatom_t struct which is being compared to atom1.
 *
 * @return an integer less than, equal to, or greater than zero.
 *
 * @sa lpatom_t, lpatom_version_cmp()
 */
extern int
lpatom_cmp(const lpatom_t *atom1, const lpatom_t *atom2);

/**
 * @brief compiles a lpatom handle into a @c null terminated C string.
 *
 * If an error occurs while compiling the string, @c NULL is returned and @c
 * errno is set to indicate the error.
 *
 * @param handle a lpatom handle.
 *
 * @return a @c null terminated C string with the compiled version.
 */
extern char *
lpatom_compile(const lpatom_t *handle);

#  ifdef __cplusplus
}
#  endif

#endif /*! LPATOM */
