/*
 * Copyright (c) 2009 Lars Hartmann All rights reserved.
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
 * @file version.h
 * @brief Functions to handle version strings.
 */
#ifndef LPVERSION
/** @cond */
#define LPVERSION 1
/** @endcond */

#  ifdef __cplusplus
extern "C" {
#  endif

#  include <regex.h>
#  include <stdint.h>

/**
 * @brief Enum for the suffixes.
 */
typedef enum {
     LPV_ALPHA = 0,
     LPV_BETA,
     LPV_PRE,
     LPV_RC,
     LPV_NO,
     LPV_P
} lpversion_sufenum_t;

/**
 * @brief struct for compiled regexes.
 */
typedef struct lpversion_regex {
     regex_t verify;            /**< @brief regex to match a valid version
                                 * string */
     regex_t suffix;            /**< @brief regex to match the suffix */
     regex_t release;           /**< @brief regex to match the release
                                 * version */
} lpversion_regex_t;

/**
 * @brief the lpversion object handle.
 *
 * This is the handle to be used by any function in this lib to store some
 * data.
 *
 * @warning do not allocate or free it yourself, use lpversion_create() and
 * lpversion_destroy().
 */
typedef struct lpversion {
     lpversion_regex_t regex;    /**< @brief a struct with compiled regexes */
     lpversion_sufenum_t suffix; /**< @brief the suffix type */
     unsigned int suffv;         /**< @brief the numerical suffix version  */
     unsigned int release;       /**< @brief the release version */
     /*@null@*/
     int64_t *va;                /**< @brief @c -1 terminated array with the
                                  * split up numerical version */
     char verc;                  /**< @brief the version character */
     /*@null@*/
     char *version;              /**< @brief the complete version string */
} lpversion_t;

/**
 * @brief returns a new lpversion_t handle.
 *
 * The returned handle needs to be initialized using lpversion_init() before
 * it can be used.
 *
 * The memory for the returned handle is allocated by malloc and is not used
 * elsewhere , if you want to destroy it, use lpversion_destroy().
 *
 * If you used the returned handle for some kind of operation and want to
 * reset it, use lpversion_reset().
 *
 * If an error occured, @c NULL is returned and @c errno is set to indicate
 * the error.
 *
 * @return a pointer to a lpversion_t handle or @c NULL if an error has
 * occured.
 *
 * @b Errors:
 *
 * - This function may fail and set errno for any of the errors specified for
 *   the routine malloc(3).
 */
/*@only@*//*@null@*//*@out@*/
extern lpversion_t *
lpversion_create(void);

/**
 * @brief initialize a lpversion_t handle.
 *
 * If you want to initialize an already used handle, use lpversion_reset()
 *
 * @param handle a lpatom_t handle to initialize
 *
 * @b Errors:
 *
 * - This function may fail and set errno for any of the errors specified for
 *   the routine malloc(3).
 */
extern void
lpversion_init(/*@special@*/lpversion_t *handle);

/**
 * @brief resets a lpversion_t handle.
 *
 * @param handle a lpatom_t handle.
 */
extern void
lpversion_reset(lpversion_t *handle);

/**
 * @brief destroy's a lpversion_t handle
 *
 * @param handle a lpversion_t handle.
 */
extern void
lpversion_destroy(lpversion_t *handle);

/**
 * @brief parses an version string.
 *
 * This parses an @c null terminated character String containing a Version,
 * eg, 2.9.18-r1, this string is validated for correctness and afterwards
 * parsed.
 *
 * If an error occurs while parsing the string, @c -1 is returned and @c errno
 * is set to indicate the error.
 *
 * @param handle a lpversion_t handle.
 * 
 * @param version a version string to parse.
 *
 * @return @c 0 if sucessfull or @c -1 if an error has occured.
 *
 * @b Errors:
 *
 * - @c EINVAL the given string is not a valid version.
 * - This function may fail and set errno for any of the errors specified for
 *   the routine malloc(3)
 * - This function may also fail and set errno for any of the errors specified
 *   for the routine strdup(3)
 */

extern int
lpversion_parse(lpversion_t *handle, char *version);

/**
 * @brief compares two lpatom_t handles.
 *
 * @param v1 a lpatom_t handle.
 *
 * @param v2 a lpatom_t handle.
 *
 * @return an integer less than, equal to, or greater than zero if v1 is
 * found, to be less than, to match, or to be greater than v2.
 */
extern int
lpversion_cmp(const lpversion_t *v1, const lpversion_t *v2);

/**
 * @brief compiles a lpversion handle into a @c null terminated C string.
 *
 * If an error occurs while compiling the string, @c NULL is returned and @c
 * errno is set to indicate the error.
 *
 * @param handle a lpversion_t handle.
 *
 * @return a @c null terminated C string with the compiled version.
 */
extern char *
lpversion_compile(const lpversion_t *handle);

#  ifdef __cplusplus
}
#  endif

#endif /*! LPVERSION */
