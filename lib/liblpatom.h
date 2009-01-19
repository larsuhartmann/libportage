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
      * _alpha
      */
     alpha,
     /**
      * _beta
      */
     beta,
     /**
      * _pre
      */
     pre,
     /**
      * _rc
      */
     rc,
     /**
      * no suffix
      */
     no,
     /**
      * _p
      */
     p
} lpatom_sufe_t;

/**
 * \brief The atom struct
 *
 * This is the data structure that holds the parsed data of a version atom.
 */
typedef struct {
     char *name;
     char *cat;
     char verc;
     char *ver;
     int *ver_ex;
     lpatom_sufe_t sufenum;
     int sufv;
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
 * \sa lpatom_t lpatom_destroy()
 *
 * \b Errors:
 * 
 * - \c EINVAL pname is not a valid package atom.
 * - This function may also fail and set errno for any of the errors specified
 *   for the routine malloc(3).
 * - This function may also fail and set errno for any of the errors specified
 *   for the routine strdup(3). 
 */
lpatom_t *
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
 * \b ATTENTION: Do not try to access the datastructure after destroying it or
 * anything can happen.
 */
void
lpatom_destroy(lpatom_t *atom);

char *
lpatom_get_qname(const lpatom_t *atom);

char *
lpatom_get_fullname(const lpatom_t *atom);

char *
lpatom_get_version(const lpatom_t *atom);

int
lpatom_version_cmp(const lpatom_t *atom1, const lpatom_t *atom2);

int
lpatom_cmp(const lpatom_t *atom1, const lpatom_t *atom2);

char *
lpatom_get_fullname(const lpatom_t *atom);

#endif
