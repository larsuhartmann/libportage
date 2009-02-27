/*
 * Copyright (c) 2009 Lars Hartmann
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

#ifndef __LPDEPENDENCY_H
#define __LPDEPENDENCY_H

#include <common.h>
#include <atom.h>

#include <stdbool.h>

BEGIN_C_DECLS

/**
 * \brief describes the dependency type
 */
typedef enum {
     /**
      * \brief equal
      * The Dependency must be equal to the dependency-atom.
      * \bExample: =foo-bar/2.8.19
      */
     eq,
     /**
      * \brief lower than
      * The Dependency must be lower than the dependency-atom.
      * \bExample: <foo-bar/2.8.19
      */
     lt,
     /**
      * \brief greater than
      * The Dependency must be greater than the dependency-atom.
      * \bExample: >foo-bar/2.8.19
      */
     gt,
     /**
      * \brief lower or equal to
      * The Dependency must be lower or equal to the dependency-atom.
      * \bExample: <=foo-bar/2.8.19
      */
     le,
     /**
      * \brief greater or equal to
      * The Dependency must be greater or equal to the dependency-atom.
      * \bExample: >=foo-bar/2.8.19
      */
     ge
} lpdependency_type_t;

typedef struct {
     lpdependency_type_t dt;
     lpatom_t *da;
} lpdependency_t;

lpdependency_t *
lpdependency_create(void);

void
lpdependency_init(lpdependency_t *dep);

int
lpdependency_parse(lpdependency_t *dep, char *s);

bool
lpdependency_match(lpdependency_t *dep, lpatom_t *atom);

lpdependency_t *
lpdependency_merge(lpdependency_t *dep1, lpdependency_t *dep2);

END_C_DECLS

#endif
