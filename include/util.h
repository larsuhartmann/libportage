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
 * @file util.h
 * @brief Functions for small tasks.
 */
#ifndef LPUTIL
/** @cond */
#define LPUTIL 1
/** @endcond */

#  include <regex.h>

#  ifdef __cplusplus
extern "C" {
#  endif

/**
 * @brief Get regexp match
 *
 * Returns a string with the given match from a array of regmatch_t
 * structs. The Memory for the returned string is obtained by malloc(3) and
 * can be freed with free(3).
 *
 * If an error occurs, @c NULL is returned and errno is set to indicate the
 * error.
 *
 * @param match a array of regmatch_t objects as used by regexec(3).
 * @param n the wanted element starting by 0 for the first element.
 * @param s the string that was matched by regexec(3).
 *
 * @return the matched string as a null terminated c string.
 * 
 * @sa regex(3)
 *
 * @b Errors:
 *
 * - @c EINVAL one or both of the given pointers are @c NULL
 * - This function may also fail and set errno for any of the errors specified
 *   for the routine malloc(3)
 */
extern char *
lputil_get_re_match(const regmatch_t *match, int n, const char *s);

/**
 * @brief Copy substring from a string.
 *
 * Copies a substring with the length len starting at off and returns it. The
 * memory for the returned String is obtained by malloc(3) and can be freed
 * with free(3).
 *
 * This function does not check if the requested match is inside the borders
 * of the given string.
 *
 * If an error occurs, @c NULL is returned and errno is set to indicate the
 * error.
 *
 * @param s the string of which we want the substring
 * @param off the start of the substring
 * @param len the length of the substring
 *
 * @return the substring as a null terminated substring.
 *
 * @b Errors:
 * - @c EINVAL s is @c NULL.
 * - This routine may also fail and set errno for any of the errors specified
 *    for the routine malloc(3).
 */
extern char *
lputil_substr(const char *s, size_t off, size_t len);

/**
 * @brief Split a String into tokens.
 *
 * Splits a string into tokens with a given delimiter. Delimiter Characters at
 * the Start or the End of the String are ignored. Memory for the returned
 * Data is obtained by malloc(3) and can be freed with free(3).
 *
 * If you want to free the returned data, you can so so by using
 * lputil_splitstr_destroy().
 *
 * If an error occurs, @c NULL is returned and errno is set to indicate the
 * error.
 *
 * @param s the string to be parsed.
 * @param delim a set of characters that delim the tokens in the parsed
 * string.
 *
 * @return a array of c strings which is terminated by the value \c NULL.
 *
 * @sa strtok(3), lputil_splitstr_destroy()
 *
 * @b Errors:
 * 
 * - \c EINVAL one or both of the given pointers are \c NULL
 * - This routine may also fail and set errno for any of the errors specified
 *   for the routine malloc(3).
 * - This routine may also fail and set errno for any of the errors specified
 *   for the routine strdup(3).
 */
extern char **
lputil_splitstr(const char *s, const char *delim);

/**
 * @brief duplicates a memory region.
 *
 * returns a pointer to a new memory region which is a duplicate of the given
 * memory region. Memory for the new region is obtained by malloc(3) and can
 * be freed with free(3).
 *
 * lputil_memdup() does not check if the memory region pointed to by s is
 * really \c len bytes long.
 *
 * If an error occurs, \c NULL is returned and errno is set to indicate the
 * error.
 *
 * @param s a pointer to the memory region to be copied.
 * @param len the length of the memory region to be copied in bytes.
 *
 * @return a pointer to a duplicate of the memory region.
 *
 * @sa strdup(3)
 *
 * @b Errors:
 *
 * - @c EINVAL s is @c NULL.
 * - This routine may also fail and set errno for any of the errors specified
 *   for the routine malloc(3)
 */
extern void *
lputil_memdup(const void *s, size_t len);

/**
 * @brief calculates the length in digits of a int.
 * 
 * @param d an integer which's length is to be calculated.
 *
 * @return the length of the integer @c d in digits.
 */
size_t
lputil_intlen(int d);

/**
 * @brief destroys an @c NULL terminated array of @c null terminated C Strings
 * as returned by lputil_splitstr().
 *
 * @param splitstr an @c NULL terminated array of @c nul terminated C Strings
 * as returned by lputil_splitstr().
 *
 * @sa lputil_splitstr()
 */
extern void
lputil_splitstr_destroy(char **splitstr);

#  ifdef __cplusplus
}
#  endif

#endif /* LPUTIL */
