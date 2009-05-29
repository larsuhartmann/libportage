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

#include <version.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

#define MAXLEN 1024

int
main(void)
{
     lpversion_t *v1;
     lpversion_t *v2;
     char *srcpath, *s, *vs1, *vs2;
     FILE *file;
     bool has_failed = false;
     
     if ( (srcpath = getenv("srcdir")) != NULL )
          if ( chdir(srcpath) == -1 )
               return EXIT_FAILURE;

     if ( (file = fopen("02_lpversion_parse.txt", "r")) == NULL )
          return EXIT_FAILURE;

     if ( (s = malloc(MAXLEN)) == NULL )
          return EXIT_FAILURE;

     if ( (v1 = lpversion_create()) == NULL )
          return EXIT_FAILURE;
     if ( (v2 = lpversion_create()) == NULL )
          return EXIT_FAILURE;

     lpversion_init(v1);
     lpversion_init(v2);
     while ( fgets(s, MAXLEN, file) != NULL ) {
          s[strlen(s)-1] = '\0';
          if ( lpversion_parse(v1, s) == -1 || lpversion_parse(v2, s) == -1 )
               has_failed = true;
          else if (lpversion_cmp(v1, v2) != 0)
               has_failed = true;
          if ( (vs1 = lpversion_compile(v1)) == NULL ||
               (vs2 = lpversion_compile(v2)) == NULL )
               has_failed = true;
          else {
               if ( strcmp(vs1, vs2) != 0 )
                    has_failed = true;
               else if (strcmp(s, vs1) != 0 || strcmp(s, vs2) != 0 )
                    has_failed = true;
               free(vs1);
               free(vs2);
          }
          lpversion_reset(v1);
          lpversion_reset(v2);
     }
     lpversion_destroy(v1);
     lpversion_destroy(v2);
     free(s);
     if ( has_failed )
          return EXIT_FAILURE;
     
     return 0;
}
