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

#include <atom.h>

#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

int main(void)
{
     lpatom_t *atom1 = NULL;
     lpatom_t *atom2 = NULL;
     char s[1024], *srcpath;
     bool has_failed = false;
     FILE *file;

     if ( (srcpath = getenv("srcdir")) != NULL )
          if ( chdir(srcpath) == -1 )
               return EXIT_FAILURE;
     
     if ( (file = fopen("02_lpatom_parse.txt", "r")) == NULL )
          return EXIT_FAILURE;

     if ( (atom1 = lpatom_create()) == NULL )
          return EXIT_FAILURE;
     lpatom_init(atom1);
     if ( (atom2 = lpatom_create()) == NULL )
          return EXIT_FAILURE;
     lpatom_init(atom2);

     while ( fgets(s, sizeof(s), file) != NULL) {
          s[strlen(s)-1] = '\0';
          if ( lpatom_parse(atom1, s) == -1)
               has_failed = true;
          if ( lpatom_parse(atom2, s) == -1)
               has_failed = true;
          if (! has_failed)
               if ( lpatom_cmp(atom1, atom2) != 0 )
                    has_failed = true;
          lpatom_reset(atom1);
          lpatom_reset(atom2);
     }
     lpatom_destroy(atom1);
     lpatom_destroy(atom2);
     fclose(file);
     if ( has_failed )
          return EXIT_FAILURE;
     return EXIT_SUCCESS;
}
