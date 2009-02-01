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
 * \file liblpatom.h
 * \brief Functions to encode/decode/parse version atoms.
 */

#ifndef __LPATOM
#define __LPATOM

#include <stdio.h>

/**
 * suffix enum
 *
 * all valid suffixes for package atoms in the right order, so they can be
 * compared directly to find out which one of two is the larger one.
 */
typedef enum {
     /**
      * \brief alpha release
      */
     alpha,
     /**
      * \brief beta release
      */
     beta,
     /**
      * \brief pre release.
      */
     pre,
     /**
      * \brief release candidate.
      */
     rc,
     /**
      * \brief standard release
      */
     no,
     /**
      * \brief production release
      */
     p
} lpatom_sufe_t;

/**
 * \brief The atom struct
 *
 * This is the data structure that holds the parsed data of a version atom.
 */
typedef struct {
     /**
      * \brief the name.
      */
     char *name;
     /**
      * \brief the category.
      */
     char *cat;
     /**
      * \brief the version character.
      */
     char verc;
     /**
      * \brief the version.
      */
     char *ver;
     /**
      * \brief the exploded version.
      */
     int *ver_ex;
     /**
      * \brief the suffix.
      */
     lpatom_sufe_t sufenum;
     /**
      * \brief the suffix version.
      */
     int sufv;
     /**
      * \brief the release version.
      */
     int rel;
} lpatom_t;

/**
 * \brief Reads the atom data out of an packetname string.
 *
 * Gets the full name of a packet as a c string and returns an pointer to an
 * lpatom_t struct which holds the parsed data.
 *
 * The given string must be a valid gentoo package name (see documentation for
 * more information)
 *
 * The memory for the returned struct is allocated by malloc and is not used
 * elsewhere in this lib, if you want to destroy it, use lpatom_destroy().
 *
 * If an error occured, \c Null is returned and \c errno is set to indicate the
 * error.
 *
 * \param pname a string with the full package version.
 *
 * \return a pointer to an lpatom_t struct or \c NULL if an error has occured.
 *
 * \sa lpatom_t, lpatom_destroy(), lpatom_get_version(),
 * lpatom_get_fullname(), lpatom_get_qname()
 *
 * \b Errors:
 * 
 * - \c EINVAL pname is not a valid package atom.
 * - This function may also fail and set errno for any of the errors specified
 *   for the routine malloc(3).
 * - This function may also fail and set errno for any of the errors specified
 *   for the routine strdup(3). 
 */
extern lpatom_t *
lpatom_parse(const char *pname);

/**
 * \brief destroys an lpatom_t object.
 *
 * frees up all corresponding memory regions using free(3). If a \c NULL
 * pointer was given, lpatom_destroy() will just return
 * 
 * \param atom a pointer to an lpxpak_t data structure that was returned by
 * lpatom_parse().
 *
 * \sa lpatom_t, lpatom_parse()
 *
 * \b ATTENTION: Do not try to access the datastructure after destroying it or
 * anything can happen.
 */
extern void
lpatom_destroy(lpatom_t *atom);

/**
 * \brief get the qualified name of a atom.
 *
 * Returns the qualified name(the category if one exists plus the name) of the
 * given atom as a \c nul terminated C string
 *
 * If an Error occurs, \c NULL is returned and \c errno is set to indicate the
 * error.
 *
 * \param atom a pointer to an lpxpak_t data structure that was returned by
 * lpatom_parse().
 *
 * \return a \c nul terminated C string with the qualified name or \c NULL if
 * an error has occured.
 *
 * \sa lpatom_t, lpatom_get_fullname(), lpatom_get_version(), lpatom_parse()
 *
 * \b Errors:
 * 
 * - This function may also fail and set errno for any of the errors specified
 *   for the routine malloc(3).
 * - This function may also fail and set errno for any of the errors specified
 *   for the routine strdup(3). 
 */
extern char *
lpatom_get_qname(const lpatom_t *atom);

/**
 * \brief get the qualified name of a atom.
 *
 * Returns the qualified name(the category if one exists plus the name) of the
 * given atom as a \c nul terminated C string.
 *
 * If an Error occurs, \c NULL is returned and \c errno is set to indicate the
 * error.
 *
 * \param atom a pointer to an lpxpak_t data structure that was returned by
 * lpatom_parse().
 *
 * \return a \c nul terminated C string with the qualified name or \c NULL if
 * an error has occured.
 *
 * \sa lpatom_t, lpatom_get_qname(), lpatom_get_version(), lpatom_parse()
 *
 * \b Errors:
 * 
 * - This function may also fail and set errno for any of the errors specified
 *   for the routine malloc(3).
 * - This function may also fail and set errno for any of the errors specified
 *   for the routine strdup(3). 
 */
extern char *
lpatom_get_fullname(const lpatom_t *atom);

/**
 * \brief get the version of a atom.
 *
 * Returns the Version (includinf suffix and release version) of the given
 * atom as a \c nul terminated C string.
 *
 * If an Error occurs, \c NULL is returned and \c errno is set to indicate the
 * error.
 *
 * \param atom a pointer to an lpxpak_t data structure that was returned by
 * lpatom_parse().
 *
 * \return a \c nul terminated C string with the version or \c NULL if an
 * error has occured.
 *
 * \sa lpatom_t, lpatom_get_qname(), lpatom_get_fullname(), lpatom_parse()
 *
 * \b Errors:
 * 
 * - This function may also fail and set errno for any of the errors specified
 *   for the routine malloc(3).
 * - This function may also fail and set errno for any of the errors specified
 *   for the routine strdup(3). 
 */
extern char *
lpatom_get_version(const lpatom_t *atom);

/**
 * \brief compare the version of two lpatom_t data structures.
 *
 * Compares the two lpatom_t structures and returns an Integer less than,
 * equal to or greater than zero if atom1 is found, respectively, to be less
 * than, to match, or greater than atom2.
 *
 * This function can directly be used in qsort(3).
 *
 * \param atom1 the first lpatom_t struct which's version is going to be
 * compared.
 *
 * \param atom2 the second lpatom_t struct which's version is being compared
 * to atom1.
 *
 * \return an integer less than, equal to, or greater than zero.
 *
 * \sa lpatom_t, lpatom_cmp()
 */
extern int
lpatom_version_cmp(const lpatom_t *atom1, const lpatom_t *atom2);

/**
 * \brief compare two lpatom_t data structures.
 *
 * Compares the two lpatom_t structures and returns an Integer less than,
 * equal to or greater than zero if atom1 is found, respectively, to be less
 * than, to match, or greater than atom2.
 *
 * This function can directly be used in qsort(3).
 *
 * \param atom1 the first lpatom_t struct which is going to be compared.
 *
 * \param atom2 the second lpatom_t struct which is being compared to atom1.
 *
 * \return an integer less than, equal to, or greater than zero.
 *
 * \sa lpatom_t, lpatom_version_cmp()
 */
extern int
lpatom_cmp(const lpatom_t *atom1, const lpatom_t *atom2);

#endif