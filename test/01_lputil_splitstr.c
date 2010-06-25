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

#include <util.h>

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define MAXLEN          1024
#define INPUTFILE       "01_lputil_splitstr.txt"

int main(void)
{
     bool has_failed = false;
     FILE *file;
     int len, i;
     char *srcpath, s[MAXLEN], delim[MAXLEN], entry[MAXLEN], t[MAXLEN], *join;
     char **split;

     if ( (srcpath = getenv("srcdir")) != NULL )
          if ( chdir(srcpath) == -1 )
               return EXIT_FAILURE;
     if ( (file = fopen(INPUTFILE, "r")) == NULL )
          return EXIT_FAILURE;
     
     while ( fgets(s, MAXLEN, file) != NULL ) {
          s[strlen(s)-1] = '\0';
          if (fgets(delim, MAXLEN, file) == NULL )
               return EXIT_FAILURE;
          delim[strlen(delim)-1] = '\0';
          if (fgets(t, MAXLEN, file) == NULL )
               return EXIT_FAILURE;
          t[strlen(t)-1] = '\0';
          len = atoi(t);
          if (fgets(entry, MAXLEN, file) == NULL )
               return EXIT_FAILURE;
          entry[strlen(entry)-1] = '\0';

          if ( (split = lputil_splitstr(s, delim)) == NULL )
               has_failed = true;
          else {
               if (strcmp(split[0], entry) != 0)
                    has_failed = true;
               for ( i=0; split[i] != NULL; ++i )
                    ;
               if ( i != len )
                    has_failed = true;
               /* check if the joined string is the same as the one we
                * originally fed to lputil_splitstr() */
               if ( (join = lputil_joinstr((const char **)split, delim))
                    == NULL )
                    has_failed = true;
               if ( strcmp(s, join) != 0 )
                    has_failed = true;
               free(join);
               lputil_splitstr_destroy(split);
          }
     }
     if (has_failed)
          return EXIT_FAILURE;
     return EXIT_SUCCESS;
}
