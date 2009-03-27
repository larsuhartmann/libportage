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

#ifndef LPARCHIVE
#define LPARCHIVE 1

#  ifdef __cplusplus
extern "C" {
#  endif

/**
 * \brief lparchive object.
 *
 * This represents a lparchive_object, it can be created using
 * lparchive_new(), initialized using lparchive_init(), reinitialized using
 * lparchive_reset() and cleaned up using lparchive_destroy().
 */
typedef struct lparchive lparchive_t;

/**
 * \brief creates a new lparchive object.
 *
 * This allocates a new lparchive object and returns it, be aware that this
 * needs to be initialized using lparchive_init() first before it can be used.
 *
 * \return a lparchive_t object.
 *
 * \warning you need to initialize this object using lparchive_init() before
 * using it!.
 */
extern lparchive_t *
lparchive_new(void);

/**
 * \brief initializes an lparchive object.
 *
 * \param handle a newly created lparchive_t object as returned by
 * lparchive_new().
 */
extern void
lparchive_init(lparchive_t *handle);

/**
 * \brief reinitializes an lparchive object.
 *
 * \param handle a used lparchive_t object.
 */
extern void
lparchive_reset(lparchive_t *handle);

/**
 * \brief destroys a lparchive object.
 *
 * \param handle an lparchive_t object.
 *
 * \warning do not try to use an destroyed object or anything could happen!
 */
extern void
lparchive_destroy(lparchive_t *handle);

/**
 * \brief open archive at \c path
 *
 * This function will automatically recognize the compression method of the
 * archive.
 *
 * \param handle a lparchive_t object.
 * \param path the path to the archive to open.
 *
 * \return \c 0 if sucessful, \c -1 if an error has occured.
 */
extern int
lparchive_open_path(lparchive_t *handle, const char *path);

/**
 * \brief open archive from file descriptor.
 *
 * This function will automatically recognize the compression method of the
 * archive.
 *
 * \param handle a lparchive_t object.
 * \param fd a file descriptor for the archive to open.
 *
 * \return \c 0 if sucessul, \c -1 if an error has occured.
 */
extern int
lparchive_open_fd(lparchive_t *handle, int fd);

/**
 * get list of all files and directories in archive.
 *
 * \param handle a lparchive_t object which was connected to an archive using
 * lparchive_open_path() or lparchive_open_fd().
 *
 * \return a \c NULL terminated array of c strings or \c NULL if an error
 * occured.
 *
 * \attention the returned array is allocated using malloc and its up to the
 * programmer to free it, for example:
 * \code
 * for ( i=0; array[i] != NULL; ++i )
 *     free(r[i]);
 * free(r);
 * \endcode
 */
extern char **
lparchive_get_entry_names(lparchive_t *handle);

/**
 * \brief extract archive to \c path.
 *
 * \param handle a lparchive_t object.
 *
 * \param path the path to where the archive will be extracted.
 *
 * \return \c 0 if successfull, \c -1 if an error occured.d
 */
extern int
lparchive_extract(lparchive_t *handle, char *path);

#  ifdef __cplusplus
}
#  endif

#endif /*! LPARCHIVE*/
