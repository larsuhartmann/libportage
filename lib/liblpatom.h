/*
 * Copyright (c) 2008, Lars Hartmann
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

#ifndef __LPATOM
#define __LPATOM

typedef struct {
     char *name;
     char *qname;
     char *cat;
     char *ver;
     char *suffix;
     int rel;
} lpatom_t;

/* 
 * lpatom_parse: Reads the atom data out of an packetname string
 *
 * Gets an version string and returns an pointer to an lpatom_t struct. If an
 * error occured, Null is returned and errno is set to indicate the error.
 *
 * Errors:
 *         EINVAL The file either is no valid gentoo binary package or has an
 *                invalid xpak.
 *
 *         The lpatom_parse() function may also fail and set errno for any of
 *         the errors specified for the routine malloc(3).
 */
lpatom_t *
lpatom_parse(const char *s);

/*
 * lpatom_destroy: destroy an lpatom_t object
 *
 * Gets an pointer to an lpatom_t object and free(2)s up all memory of that
 * object. If a NULL pointer was given, lpatom_destroy will just return.
 *
 * ATTENTION: Do not try to use a destroyed __lpatom_t object or unexpected
 *            things will happen
 */
void
lpatom_destroy(lpatom_t *atom);

#endif
