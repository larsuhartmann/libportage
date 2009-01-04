/*
 * Copyright (c) 2008-2009 Lars Hartmann
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

/**
 * \brief A xpak element
 *
 * This is the data structure that holds one element of the xpak.
 *
 * None of the Pointers in this struct will point to memory regions which are
 * used elsewhere by the lpxpak library.
 */
typedef struct lpxpak{
     /**
      * \brief The Name of the element.
      *
      * This element is implemented as a null terminated C-String.
      */
     char *name;
     /**
      * \brief The length of the value.
      *
      * This element holds the length of the data, the value pointer points
      * to.
      */
     size_t value_len;
     /**
      * \brief A pointer to the value - a value_len bytes long memory block.
      */
     void *value;
} lpxpak_t;

/**
 * \brief Reads the xpak data out of a xpak binary blob.
 *
 * Gets the actual xpak-blob (see doc/xpak.txt) and returns a \c NULL
 * terminated array of pointers to lpxpak structures which holds the parsed
 * data. If an error occurs, NULL is returned and errno is set to indicate the
 * error.
 *
 * The returned array can be freed with lpxpak_destroy_xpak().
 *
 * \param a pointer to the start of a memory region which holds the xpak blob.
 * 
 * \param len the length of the xpak pointed to by data.
 * 
 * \return a \c NULL terminated array of pointers to lpxpak_t structures which
 * holds the parsed xpak data or \c NULL, if an error has occured.
 * 
 * \sa lpxpak_t
 * \sa lpxpak_destroy_xpak()
 *
 * \b Errors:
 * 
 * - \c EINVAL The file either is no valid gentoo binary package or has an
 *   invalid xpak.
 * - \c EBUSY The xpak could not be fully read in.
 * 
 * - This function may also fail and set errno for any of the errors specified
 *   for the routine malloc(3).
 * - This function may also fail and set errno for any of the errors specified
 *   for the routine strdup(3).
 */
lpxpak_t **
lpxpak_parse_data(const void *data, size_t len);

/**
 * \brief Reads the xpak data out of a file-descriptor.
 *
 * Gets an file-descriptor (fd) for a Gentoo binary package and returns a \c
 * NULL terminated array of pointers to lpxpak structures which holds the
 * parsed data. If an error occurs, NULL is returned and errno is set to
 * indicate the error.
 *
 * The returned array can be freed with lpxpak_destroy_xpak().
 * 
 * \param fd a file descriptor with the gentoo binary package which needs to
 *        be opened in O_RDONLY mode.
 * \return a \c NULL terminated array of pointers to lpxpak_t structures which
 * holds the parsed xpak data or \c NULL, if an error has occured.
 *
 * \sa lpxpak_t
 * \sa lpxpak_destroy_xpak()
 *
 * \b Errors:
 * 
 * - \c EINVAL The file either is no valid gentoo binary package or has an
 *   invalid xpak.
 * - \c EBUSY The xpak could not be fully read in.
 * - This function may also fail and set errno for any of the errors specified
 *   for the routine malloc(3).
 * - This function may also fail and set errno for any of the errors specified
 *   for the routine strdup(3).
 * - This function may also fail and set errno for any of the errors specified
 *   for the routine lseek(2).
 * - This function may also fail and set errno for any of the errors specified
 *   for the routine read(2).
 */
lpxpak_t **
lpxpak_parse_fd(int fd);

/**
 * \brief Reads the xpak data out of a FILE buffer.
 *
 * Gets a pointer to a FILE buffer for a Gentoo binary package and returns a
 * \c NULL terminated array of pointers to lpxpak structures which holds the
 * parsed data. If an error occurs, NULL is returned and errno is set to
 * indicate the error.
 *
 * The returned array can be freed with lpxpak_destroy_xpak().
 *
 * \param file a pointer to a FILE buffer with the gentoo binary package which
 *        needs to be opened in "r" mode.  \return a pointer to a lpxpak_t
 *        data structure which holds the parsed xpak data.
 *        
 * \return a \c NULL terminated array of pointers to lpxpak_t structures which
 * holds the parsed xpak data or \c NULL, if an error has occured.
 *
 * \sa lpxpak_t
 * \sa lpxpak_destroy_xpak()
 *
 * \b Errors:
 * 
 * - \c EINVAL The file either is no valid gentoo binary package or has an
 *   invalid xpak.
 * - \c EBUSY The xpak could not be fully read in.
 * - This function may also fail and set errno for any of the errors specified
 *   for the routine malloc(3).
 * - This function may also fail and set errno for any of the errors specified
 *   for the routine strdup(3).
 * - This function may also fail and set errno for any of the errors specified
 *   for the routine lseek(2).
 * - This function may also fail and set errno for any of the errors specified
 *   for the routine read(2).
 * - This function may also fail and set errno for any of the errors specified
 *   for the routine fileno(3).
 */
lpxpak_t **
lpxpak_parse_file(FILE *file);

/**
 * \brief Reads the xpak data out of a Gentoo binary package.
 *
 * Gets an path to a Gentoo binary package and returns a \c NULL terminated
 * array of pointers to lpxpak structures which holds the parsed data. If an
 * error occurs, NULL is returned and errno is set to indicate the error.
 *
 * The returned array can be freed with lpxpak_destroy_xpak().
 * 
 * \param path Path to a gentoo binary package which needs to be
 *        readable by the current process.
 *
 * \return a \c NULL terminated array of pointers to lpxpak_t structures which
 * holds the parsed xpak data or \c NULL, if an error has occured.
 *
 * \sa lpxpak_t
 * \sa lpxpak_destroy_xpak()
 * 
 * \b Errors:
 *
 * - \c EINVAL The file either is no valid gentoo binary package or has an
 *   invalid xpak.
 * - \c EBUSY The xpak could not be fully read in.
 * - This function may also fail and set errno for any of the errors specified
 *   for the routine malloc(3).
 * - This function may also fail and set errno for any of the errors specified
 *   for the routine strdup(3).
 * - This function may also fail and set errno for any of the errors specified
 *   for the routine lseek(2).
 * - This function may also fail and set errno for any of the errors specified
 *   for the routine read(2).
 * - This function may also fail and set errno for any of
 *   the errors specified for the routine open(2).
 */
lpxpak_t **
lpxpak_parse_path(const char *path);

/**
 * \brief Destroy an \c NULL terminated array of pointers to
 * lpxpak_t structures.
 *
 * frees up the array and all of the underlying structures plus the pointers
 * they have using free(3). If a \c NULL pointer was given,
 * lpxpak_destroy_xpak() will just return.
 *
 * \param a \c NULL terminated array of pointers to lpxpak_t structures.
 *
 * \b Attention: Do not try to access the array after destroying it or
 * anything can happen.
 */
void
lpxpak_destroy_xpak(lpxpak_t **xpak);

#endif
