/*
 * Copyright (c) 2008, Lars Hartmann
 * All rights reserved.
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

#ifndef __LPXPAK
#define __LPXPAK

#include <sys/types.h>
#include <stdint.h>
#include <stdio.h>

typedef struct lpxpak{
     const char *name;
     size_t value_len;
     const void *value;
     const struct lpxpak *next;
} lpxpak_t;

/* 
 * lpxpak_parse_data: Reads the xpak data out of a xpak binary blob.
 *
 * Gets the actual xpak-blob (see doc/xpak.txt) and returns a pointer to an
 * lpxpak object with its data. If an error occurs, NULL is returned and errno
 * is set to indicate the error.
 *
 * Errors:
 *         EINVAL The file either is no valid gentoo binary package or has an
 *                invalid xpak.
 *
 *         EBUSY The xpak could not be fully read in
 *
 *         The lpxpak_parse_data() function may also fail and set errno for
 *         any of the errors specified for the routine malloc(3).
 *
 *         The lpxpak_parse_data() function may also fail and set errno for
 *         any of the errors specified for the routine strdup(3)
 */
lpxpak_t *
lpxpak_parse_data(const void *data, size_t len);

/* 
 * lpxpak_parse_fd: Reads the xpak data out of a file-descriptor which points
 *                  to an Gentoo binary package.
 *
 * Gets an file-descriptor (fd) for a Gentoo binary package and returns an
 * pointer to an lpxpak object with the xpak data. If an error occurs, NULL is
 * returned and errno is set to indicate the error.
 *
 * Errors:
 *         EINVAL The file either is no valid gentoo binary package or has an
 *                invalid xpak.
 *
 *         EBUSY The xpak could not be fully read in
 *
 *         The lpxpak_parse_fd() function may also fail and set errno for any
 *         of the errors specified for the routine malloc(3).
 *
 *         The lpxpak_parse_fd() function may also fail and set errno for
 *         any of the errors specified for the routine strdup(3)
 *
 *         The lpxpak_parse_fd() function may also fail and set errno for any
 *         of the errors specified for the routine lseek(2).
 *
 *         The lpxpak_parse_fd() function may also fail and set errno for any
 *         of the errors specified for the routine read(2).
 */
lpxpak_t *
lpxpak_parse_fd(int fd);

/* 
 * lpxpak_parse_file: Reads the xpak data out of a FILE * buffer which points
 *                    to an Gentoo binary package.
 *
 * Gets an FILE buffer for a Gentoo binary package and returns a pointer to
 * an lpxpak object with the xpak data. If an error occurs, NULL is returned
 * and errno is set to indicate the error.
 *
 * Errors:
 *         EINVAL The file either is no valid gentoo binary package or has an
 *                invalid xpak.
 *
 *         EBUSY The xpak could not be fully read in
 *
 *         The lpxpak_parse_file() function may also fail and set errno for
 *         any of the errors specified for the routine malloc(3).
 *
 *         The lpxpak_parse_file() function may also fail and set errno for
 *         any of the errors specified for the routine strdup(3)
 *
 *         The lpxpak_parse_file() function may also fail and set errno for
 *         any of the errors specified for the routine lseek(2).
 *
 *         The lpxpak_parse_file() function may also fail and set errno for
 *         any of the errors specified for the routine read(2).
 *
 *         The lpxpak_parse_file() function may also fail and set errno for
 *         any of the errors specified for the routine fileno(3).
 */
lpxpak_t *
lpxpak_parse_file(FILE *file);

/* 
 * lpxpak_parse_path: Reads the xpak data out of a Gentoo binary packages'
 *                    path it was called with.
 *
 * Gets an path to a Gentoo binary package and returns a pointer to an lpxpak
 * object with the xpak data. If an error occurs, NULL is returned and errno
 * is set to indicate the error.
 *
 * Errors:
 *         EINVAL The file either is no valid gentoo binary package or has an
 *                invalid xpak.
 *                
 *         EBUSY The xpak could not be fully read in
 *
 *         The lpxpak_parse_path() function may also fail and set errno for
 *         any of the errors specified for the routine malloc(3).
 *
 *         The lpxpak_parse_path() function may also fail and set errno for
 *         any of the errors specified for the routine strdup(3)
 *
 *         The lpxpak_parse_path() function may also fail and set errno for
 *         any of the errors specified for the routine lseek(2).
 *
 *         The lpxpak_parse_path() function may also fail and set errno for
 *         any of the errors specified for the routine read(2).
 *
 *         The lpxpak_parse_path() function may also fail and set errno for
 *         any of the errors specified in the routine open(2).
 */
lpxpak_t *
lpxpak_parse_path(const char *path);

/*
 * lpxpak_destroy_xpak: destroy an xpak object
 *
 * Gets a pointer to an lpxpak_t object and free(2)s up all memory of that
 * object. If a NULL pointer was given, lpxpak_destroy_xpak will just return.
 *
 * ATTENTION: Do not try to use a destroyed xpak object or unexpected things
 *            will happen.
 */
void
lpxpak_destroy_xpak(lpxpak_t *xpak);

#endif
