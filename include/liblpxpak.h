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

/**
 * \file liblpxpak.h
 * \brief Functions to decode/encode/manipulate xpaks.
 *
 * In here you ll find many functions to deal with xpak files (the binary
 * thing that is attached to every gentoo binary package). If you are seeking
 * for more information regarding the xpak data format, look at doc/xpak.txt
 */

#ifndef __LPXPAK
#define __LPXPAK

#include <sys/types.h>
#include <stdint.h>

/**
 * \brief A xpak element
 *
 * This is the data structure that holds one element of the xpak.
 *
 * None of the Pointers in this struct will point to memory regions which are
 * used elsewhere by the lpxpak library.
 *
 * \sa lpxpak_destroy()
 */
typedef struct {
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
 * \brief A xpak blob.
 *
 * This is the data structure that holds binary data just as a compiled
 * xpak-blob as attached to gentoo binary packages.
 */
typedef struct {
     /**
      * \brief The data.
      *
      * a Pointer to the start of a memory segment that holds the data.
      */
     void *data;
     /**
      * \brief the length of the data.
      *
      * the length of the memory block pointed to by data.
      */
     size_t len;
} lpxpak_blob_t;

/**
 * \brief Reads the xpak data out of a xpak binary blob.
 *
 * Gets the actual xpak-blob (see doc/xpak.txt) and returns a \c NULL
 * terminated array of pointers to lpxpak structures which holds the parsed
 * data. If an error occurs, NULL is returned and errno is set to indicate the
 * error.
 *
 * The returned array can be freed with lpxpak_destroy().
 *
 * \param blob a pointer to an lpxpak_blob_t data structure which holds the
 * xpak blob.
 * 
 * \return a \c NULL terminated array of pointers to lpxpak_t structures which
 * holds the parsed xpak data or \c NULL, if an error has occured.
 * 
 * \sa lpxpak_t lpxpak_destroy()
 *
 * \b Errors:
 * 
 * - \c EINVAL The file either is no valid gentoo binary package or has an
 *   invalid xpak.
 * - This function may also fail and set errno for any of the errors specified
 *   for the routine malloc(3).
 * - This function may also fail and set errno for any of the errors specified
 *   for the routine strdup(3).
 */
extern lpxpak_t **
lpxpak_parse_data(const lpxpak_blob_t *blob);

/**
 * \brief Reads the xpak data out of a file-descriptor.
 *
 * Gets an file-descriptor (fd) for a Gentoo binary package and returns a \c
 * NULL terminated array of pointers to lpxpak structures which holds the
 * parsed data. If an error occurs, NULL is returned and errno is set to
 * indicate the error.
 *
 * The returned array can be freed with lpxpak_destroy().
 * 
 * \param fd a file descriptor with the gentoo binary package which needs to
 *        be opened in O_RDONLY mode.
 * \return a \c NULL terminated array of pointers to lpxpak_t structures which
 * holds the parsed xpak data or \c NULL, if an error has occured.
 *
 * \sa lpxpak_t
 * \sa lpxpak_destroy()
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
extern lpxpak_t **
lpxpak_parse_fd(int fd);

/**
 * \brief Reads the xpak data out of a Gentoo binary package.
 *
 * Gets an path to a Gentoo binary package and returns a \c NULL terminated
 * array of pointers to lpxpak structures which holds the parsed data. If an
 * error occurs, NULL is returned and errno is set to indicate the error.
 *
 * The returned array can be freed with lpxpak_destroy().
 * 
 * \param path Path to a gentoo binary package which needs to be readable by
 *        the current process.
 *
 * \return a \c NULL terminated array of pointers to lpxpak_t structures which
 * holds the parsed xpak data or \c NULL, if an error has occured.
 *
 * \sa lpxpak_t
 * \sa lpxpak_destroy()
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
extern lpxpak_t **
lpxpak_parse_path(const char *path);

/**
 * \brief Destroys an \c NULL terminated array of pointers to lpxpak_t
 * structures.
 *
 * frees up the array and all of the underlying structures plus the pointers
 * they have using free(3). If a \c NULL pointer was given, lpxpak_destroy()
 * will just return.
 *
 * \param xpak a \c NULL terminated array of pointers to lpxpak_t structures.
 *
 * \b Attention: Do not try to access the array after destroying it or
 * anything can happen.
 */
extern void
lpxpak_destroy(lpxpak_t **xpak);

/**
 * \brief Returns the value for a given key.
 *
 * Searches the provided array for an entry with the given key and returns it,
 * if no corresponding element could be found, \c NULL is returned.
 *
 * \param xpak a \c NULL terminated array of pointers to lpxpak_t objects
 * which as returned by lpxpak_parse_fd().
 * \param key a \c null terminated C string with the key to search for.
 *
 * \return a pointer to a lpxpak_t object or NULL if no corresponding element
 * could be found
 *
 * \sa lpxpak_parse_fd()
 */
extern lpxpak_t *
lpxpak_get(lpxpak_t **xpak, char *key);

/**
 * \brief Reads the xpak data out of a Gentoo binary package.
 *
 * Gets an file-descriptor (fd) for a Gentoo binary package and returns a
 * pointer to an lpxpak_blob_t data structure which holds the xpak blob.
 *
 * If an error occurs, NULL is returned and errno is set to indicate the
 * error.
 *
 * The returned data structure can be freed with lpxpak_blob_destroy().
 * 
 * \param fd a file descriptor with the gentoo binary package which needs to
 *        be opened in O_RDONLY mode.
 *        
 * \return a pointer to an lpxpak_blob_t data structure which holds the parsed
 * xpak data or \c NULL, if an error has occured.
 *
 * \sa lpxpak_blob_t lpxpak_blob_destroy()
 * 
 * \b Errors:
 *
 * - \c EINVAL The file either is no valid gentoo binary package or has an
 *   invalid xpak.
 * - \c EBUSY The xpak could not be fully read in.
 * - This function may also fail and set errno for any of the errors specified
 *   for the routine malloc(3).
 * - This function may also fail and set errno for any of the errors specified
 *   for the routine lseek(2).
 * - This function may also fail and set errno for any of the errors specified
 *   for the routine read(2).
 */
extern lpxpak_blob_t *
lpxpak_blob_get_fd(int fd);

/**
 * \brief Allocates a new lpxpak_blob_t structure.
 *
 * This function allocates a new lpxpak_blob_t data structure.
 * 
 * The returned data structure can be initialized by lpxpak_blob_init().
 *
 * If an error occurs, \c NULL is returned and errno is set.
 *
 * The Memory of the returned data structure and all its members can be freed
 * with lpxpak_blob_destroy().
 *
 * \return a pointer to an lpxpak_blob_t data structure or \c NULL if an error
 * occured.
 *
 * \sa lpxpak_blob_init() lpxpak_blob_destroy()
 * 
 * \b Errors:
 *
 * - This function may fail and set errno for any of the errors specified for
 * the routine malloc(3).
 */
extern lpxpak_blob_t *
lpxpak_blob_create(void);

/**
 * \brief Initialises a new lpxpak_blob_t structure.
 *
 * This function sets all values of the structure to \c 0 and all pointers to
 * \¢ NULL.
 *
 * 
 * \param blob a pointer to an lpxpak_blob_t data structure.
 *
 * \sa lpxpak_blob_create() lpxpak_blob_destroy()
 */
extern void
lpxpak_blob_init(lpxpak_blob_t *blob);

/**
 * \brief Destroys an lpxpak_blob_t data structure using free(3). If a \c NULL
 * pointer was given, lpxpak_destroy() will just return.
 *
 * \param xpak a pointer to an lpxpak_blob_t data structure.
 *
 * \b Attention: Do not try to access the data structure after destroying it
 * or anything can happen.
 */
extern void
lpxpak_blob_destroy(lpxpak_blob_t *blob);


/**
 * \brief Reads the xpak data out of a Gentoo binary package.
 *
 * Gets an path to a Gentoo binary package and returns a pointer to an
 * lpxpak_blob_t data structure which holds the xpak blob.
 *
 * If an error occurs, NULL is returned and errno is set to indicate the
 * error.
 *
 * The returned data structure can be freed with lpxpak_blob_destroy().
 * 
 * \param path Path to a gentoo binary package which needs to be readable by
 *        the current process.
 *        
 * \return a pointer to an lpxpak_blob_t data structure which holds the parsed
 * xpak data or \c NULL, if an error has occured.
 *
 * \sa lpxpak_blob_t lpxpak_blob_destroy()
 * 
 * \b Errors:
 *
 * - \c EINVAL The file either is no valid gentoo binary package or has an
 *   invalid xpak.
 * - \c EBUSY The xpak could not be fully read in.
 * - This function may also fail and set errno for any of the errors specified
 *   for the routine malloc(3).
 * - This function may also fail and set errno for any of the errors specified
 *   for the routine lseek(2).
 * - This function may also fail and set errno for any of the errors specified
 *   for the routine read(2).
 */
extern lpxpak_blob_t *
lpxpak_blob_get_path(const char *path);

/**
 * \brief compiles an \c NULL terminated array of lpxpak_t structs to a xpak
 * binary-blob.
 *
 * If an error occurs, \c NULL is returned and \c errno is set to indicate the
 * error.
 *
 * \param xpak a \c NULL terminated array of lpxpak_t structs.
 *
 * \return a pointer to a lpxpak_blob_t data structure with the compiled xpak.
 *
 * \sa lpxpak_blob_t lpxpak_blob_destroy()
 * 
 * \b Errors:
 * 
 * - This function may fail and set errno for any of the errors specified for
 *   the routine malloc(3).
 */
extern lpxpak_blob_t *
lpxpak_blob_compile(lpxpak_t **xpak);
#endif
