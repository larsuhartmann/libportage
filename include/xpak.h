/*
 * Copyright (c) 2008-2009 Lars Hartmann All rights reserved.
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
 * @file xpak.h
 * @brief Functions to handle xpak blobs.
 */
#ifndef LPXPAK
/** @cond */
#define LPXPAK 1
/** @endcond */

#  include <sys/types.h>
#  include <stdint.h>

#  ifdef __cplusplus
extern "C" {
#  endif

/**
 * @brief A xpak entry
 *
 * This is the data structure that holds one element of the xpak.
 *
 * None of the Pointers in this struct will point to memory regions which are
 * used elsewhere by the lpxpak library.
 */
typedef struct lpxpak_entry {
     /**
      * @brief The length of the value.
      *
      * This element holds the length of the data, the value pointer points
      * to.
      */
     size_t value_len;
     /**
      * @brief The Name of the element.
      *
      * This element is implemented as a null terminated C-String.
      */
     char *name;
     /**
      * @brief A pointer to the value - a value_len bytes long memory block.
      */
     void *value;
} lpxpak_entry_t;

/**
 * @brief The xpak data structure.
 *
 * This is the datastructure used as a handle in the xpak related functions of
 * libportage. To Create an Handle you can either malloc it yourself or use
 * lpxpak_create() instead.
 *
 * Beware that you need to initialize an handle using lpxpak_init() first
 * before you use it with any functions provided by libportage.
 *
 * None of the Pointers in this struct will point to memory regions which are
 * used elsewhere by the lpxpak library.
 *
 *  @sa lpxpak_create(), lpxpak_init(), lpxpak_reinit(), lpxpak_destroy().
 */
typedef struct lpxpak {
     /**
      * The length of the array entries.
      */
     size_t size;
     /**
      * A pointer to an array of holding @c len lpxpak_entry_t data
      * structures.
      */
     lpxpak_entry_t *entries;
} lpxpak_t;

/**
 * @brief A xpak blob.
 *
 * This is the data structure that holds binary data just as a compiled
 * xpak-blob as attached to gentoo binary packages.
 *
 * To savely Remove an lpxpak_blob_t datastructure and all the data it points
 * to from memory, use lpxpak_blob_destroy().
 *
 * @sa lpxpak_blob_create(), lpxpak_blob_init(), lpxpak_blob_destroy().
 */
typedef struct lpxpak_blob {
     /**
      * @brief the length of the data.
      *
      * the length of the memory block pointed to by data.
      */
     size_t len;
     /**
      * @brief The data.
      *
      * a Pointer to the start of a memory segment that holds the data.
      */
     void *data;
} lpxpak_blob_t;

/**
 * @brief Reads the xpak data out of a xpak binary blob.
 *
 * Gets the actual xpak-blob (see doc/xpak.txt) and parses its content into a
 * provided lpxpak_t handle. If an error occurs, -1 is returned and errno is
 * set to indicate the error.
 *
 * @param handle a pointer to an lpxpak_t data structure used for this
 * operation.
 * 
 * @param blob a pointer to an lpxpak_blob_t data structure which holds the
 * xpak blob.
 * 
 * @return @c 0 if successfull or @c -1 if an error occured.
 * 
 * @sa lpxpak_t lpxpak_create(), lpxpak_init(), lpxpak_destroy().
 *
 * @b Errors:
 * 
 * - @c EINVAL The file either is no valid gentoo binary package or has an
 *   invalid xpak.
 * - This function may also fail and set errno for any of the errors specified
 *   for the routine malloc(3).
 * - This function may also fail and set errno for any of the errors specified
 *   for the routine strdup(3).
 */
extern int
lpxpak_parse_data(lpxpak_t *handle, const lpxpak_blob_t *blob);

/**
 * @brief Reads the xpak data out of a file-descriptor.
 *
 * Gets an file-descriptor (fd) for a Gentoo binary package and parses it into
 * a provided lpxpak_t handle. If an error occurs, @c -1 is returned and errno
 * is set to indicate the error.
 *
 * @param handle a pointer to an lpxpak_t data structure used for this
 * operation.
 * 
 * @param fd a file descriptor with the gentoo binary package which needs to
 * be opened in O_RDONLY mode.
 *
 * @return 0 if successfull or -1 if an error occured.
 *
 * @sa lpxpak_t
 * @sa lpxpak_destroy()
 *
 * @b Errors:
 * 
 * - @c EINVAL The file either is no valid gentoo binary package or has an
 *   invalid xpak.
 * - @c EBUSY The xpak could not be fully read in.
 * - @c EBADFD The file descriptor is not opened in read-mode.
 * - This function may also fail and set errno for any of the errors specified
 *   for the routine malloc(3).
 * - This function may also fail and set errno for any of the errors specified
 *   for the routine strdup(3).
 * - This function may also fail and set errno for any of the errors specified
 *   for the routine lseek(2).
 * - This function may also fail and set errno for any of the errors specified
 *   for the routine read(2).
 */
extern int
lpxpak_parse_fd(lpxpak_t *handle, int fd);

/**
 * @brief Reads the xpak data out of a Gentoo binary package.
 *
 * Gets an path to a Gentoo binary package and parses it into the provided
 * lpxpak_t handle. If an error occurs, -1 is returned and errno is set to
 * indicate the error.
 *
 * @param handle a pointer to an lpxpak_t data structure used for this
 * operation.
 * 
 * @param path Path to a gentoo binary package which needs to be readable by
 * the current process.
 *
 * @return 0 if successfull or -1 if an error occured.
 *
 * @sa lpxpak_t
 * @sa lpxpak_destroy()
 * 
 * @b Errors:
 *
 * - @c EINVAL The file either is no valid gentoo binary package or has an
 *   invalid xpak.
 * - @c EBUSY The xpak could not be fully read in.
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
extern int
lpxpak_parse_path(lpxpak_t *handle, const char *path);

/**
 * @brief Allocates a new lpxpak_t handle.
 *
 * If an error occurs, @c NULL is returned and errno is set.
 *
 * The returned data structure can be initialized by lpxpak_init().
 *
 * The returned handle can be reinitialised for further use using
 * lpxpak_reinit().
 *
 * The Memory for the returned array including all its members can be freed
 * with lpxpak_destroy().
 *
 * @return a @c NULL terminated array of lpxpak_t structures or @c NULL if an
 * error occured.
 *
 * @sa lpxpak_init(), lpxpak_reinit(), lpxpak_destroy().
 * 
 * @b Errors:
 *
 * - This function may fail and set errno for any of the errors specified for
 * the routine malloc(3).
 */
extern lpxpak_t *
lpxpak_create(void);

/**
 * @brief Initialises a lpxpak_t handle.
 *
 * This function sets all values of the structure to @c 0 and all pointers to
 * @c NULL.
 *
 * @param xpak a lpxpak_t handle.
 *
 * @sa lpxpak_create(), lpxpak_reinit(),  lpxpak_destroy().
 */
extern void
lpxpak_init(lpxpak_t *xpak);

/**
 * @brief Destroys an lpxpak_t handle.
 *
 * frees up the handle and all of the underlying structures plus the pointers
 * they have using free(3). If a @c NULL pointer was given, lpxpak_destroy()
 * will just return.
 *
 * @param xpak a lpxpak_t handle.
 *
 * @warning Do not try to access the array after destroying it or anything can
 * happen.
 */
extern void
lpxpak_destroy(lpxpak_t *xpak);

/**
 * @brief Returns the xpak entry for a given key.
 *
 * Searches the provided array for an entry with the given key and returns it,
 * if no corresponding element could be found, @c NULL is returned.
 *
 * @param handle a lpxpak_t handle with parsed data.
 * 
 * @param key a @c null terminated C string with the key to search for.
 *
 * @return a pointer to a lpxpak_entry_t object with the corresponding value.
 *
 * @sa lpxpak_parse_fd()
 */
extern lpxpak_entry_t *
lpxpak_get(lpxpak_t *handle, char *key);

/**
 * @brief Reads the xpak data out of a Gentoo binary package.
 *
 * Gets an file-descriptor (fd) for a Gentoo binary package and returns a
 * pointer to an lpxpak_blob_t data structure which holds the xpak blob.
 *
 * If an error occurs, NULL is returned and errno is set to indicate the
 * error.
 *
 * The returned data structure can be freed with lpxpak_blob_destroy().
 * 
 * @param fd a file descriptor with the gentoo binary package which needs to
 * be opened in O_RDONLY mode.
 *
 * @return a pointer to an lpxpak_blob_t data structure which holds the parsed
 * xpak data or @c NULL, if an error has occured.
 *
 * @sa lpxpak_blob_t lpxpak_blob_destroy()
 * 
 * @b Errors:
 *
 * - @c EINVAL The file either is no valid gentoo binary package or has an
 *   invalid xpak.
 * - @c EBUSY The xpak could not be fully read in.
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
 * @brief Allocates a new lpxpak_blob_t structure.
 *
 * This function allocates a new lpxpak_blob_t data structure.
 * 
 * The returned data structure can be initialized by lpxpak_blob_init().
 *
 * If an error occurs, @c NULL is returned and errno is set.
 *
 * The Memory of the returned data structure and all its members can be freed
 * with lpxpak_blob_destroy().
 *
 * @return a pointer to an lpxpak_blob_t data structure or @c NULL if an error
 * occured.
 *
 * @sa lpxpak_blob_init() lpxpak_blob_destroy()
 * 
 * @b Errors:
 *
 * - This function may fail and set errno for any of the errors specified for
 * the routine malloc(3).
 */
extern lpxpak_blob_t *
lpxpak_blob_create(void);

/**
 * @brief Initialises a new lpxpak_blob_t structure.
 *
 * This function sets all values of the structure to @c 0 and all pointers to
 * @c NULL.
 * 
 * @param blob a pointer to an lpxpak_blob_t data structure.
 *
 * @sa lpxpak_blob_create() lpxpak_blob_destroy()
 */
extern void
lpxpak_blob_init(lpxpak_blob_t *blob);

/**
 * @brief Destroys an lpxpak_blob_t data structure using free(3). If a @c NULL
 * pointer was given, lpxpak_destroy() will just return.
 *
 * @param blob a pointer to an lpxpak_blob_t data structure.
 *
 * @warning Do not try to access the data structure after destroying it or
 * anything can happen.
 */
extern void
lpxpak_blob_destroy(lpxpak_blob_t *blob);

/**
 * @brief Reads the xpak data out of a Gentoo binary package.
 *
 * Gets an path to a Gentoo binary package and returns a pointer to an
 * lpxpak_blob_t data structure which holds the xpak blob.
 *
 * If an error occurs, @c NULL is returned and errno is set to indicate the
 * error.
 *
 * The returned data structure can be freed with lpxpak_blob_destroy().
 * 
 * @param path Path to a gentoo binary package which needs to be readable by
 * the current process.
 *
 * @return a pointer to an lpxpak_blob_t data structure which holds the parsed
 * xpak data or @c NULL, if an error has occured.
 *
 * @sa lpxpak_blob_t lpxpak_blob_destroy()
 * 
 * @b Errors:
 *
 * - @c EINVAL The file either is no valid gentoo binary package or has an
 *   invalid xpak.
 * - @c EBUSY The xpak could not be fully read in.
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
 * @brief compiles an lpxpak_t object with parsed data into an xpak blob.
 *
 * If an error occurs, @c NULL is returned and @c errno is set to indicate the
 * error.
 *
 * @param handle a lpxpak_t handle with parsed data.
 *
 * @return a pointer to a lpxpak_blob_t data structure with the compiled xpak.
 *
 * @sa lpxpak_blob_t lpxpak_blob_destroy()
 * 
 * @b Errors:
 * 
 * - This function may fail and set errno for any of the errors specified for
 *   the routine malloc(3).
 */
extern lpxpak_blob_t *
lpxpak_blob_compile(lpxpak_t *handle);

#  ifdef __cplusplus
}
#  endif

#endif /* LPXPAK */
