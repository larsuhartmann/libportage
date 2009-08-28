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

#include <archives.h>

#include <sys/stat.h>
#include <sys/types.h>

#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>

#define TESTFILE        "05_lparchives.tbz2"
#define TESTFILECONTENT "05_lparchives.txt"
#define MAXLEN          1024

int
test_lparchives_extract(bool distcheck);

int
test_lparchives_get_entry_names();

int main(void)
{
     char *srcpath;
     bool distcheck=false;

     if ( (srcpath = getenv("srcdir")) != NULL ) {
          if ( chdir(srcpath) == -1 )
               return EXIT_FAILURE;
          distcheck = true;
     }
     if ( test_lparchives_get_entry_names() == -1 )
          return EXIT_FAILURE;
     if ( test_lparchives_extract(distcheck) == -1 )
          return EXIT_FAILURE;
     return EXIT_SUCCESS;
}

int
test_lparchives_extract(bool distcheck)
{
     FILE *file = NULL;
     int fd = 0;
     lparchive_t *archive = NULL;
     char buf[MAXLEN], cwd[MAXLEN];
     struct stat statstruct;
     bool has_failed = false;
     if ( (file = fopen(TESTFILECONTENT, "r")) == NULL )
          goto test_lparchives_extract_bailout;

     if ( (fd = open(TESTFILE, O_RDONLY)) == -1 )
          goto test_lparchives_extract_bailout;

     if ( (archive = lparchive_new()) == NULL )
          goto test_lparchives_extract_bailout;

     lparchive_init(archive);
     if ( (lparchive_open_fd(archive, fd)) == -1)
          goto test_lparchives_extract_bailout;
     getcwd(cwd, MAXLEN);

     if ( distcheck )
          chmod(cwd, S_IRUSR|S_IWUSR|S_IXUSR);
     
     mkdir("test", 0777);

     if ( lparchive_extract(archive, "test") == -1 )
          goto test_lparchives_extract_bailout;
     chdir("test");
     fgets(buf, MAXLEN, file);
     while (fgets(buf, MAXLEN, file) != NULL) {
          buf[strlen(buf)-1] = '\0';
          if ( stat(buf, &statstruct) == -1) {
               has_failed = true;
               break;
          }
          if ( buf[strlen(buf)-1] == '/' ) {
               if (! S_ISDIR(statstruct.st_mode) ) {
                    has_failed = true;
                    break;
               }
          } else if (! S_ISREG(statstruct.st_mode ) ) {
               has_failed = true;
               break;
          }
     }
     
     chdir(cwd);
     remove("test");

     fclose(file);
     close(fd);
     lparchive_destroy(archive);

     if ( has_failed )
          return -1;
     
     return 0;

test_lparchives_extract_bailout:
     if ( file )
          fclose(file);
     if ( fd )
          close(fd);
     if ( archive )
          lparchive_destroy(archive);
     remove("test");
     return -1;
}

int
test_lparchives_get_entry_names()
{
     FILE *file = NULL;
     int fd = 0;
     lparchive_t *archive = NULL;
     char **r, buf[MAXLEN];
     size_t len;
     int i;
     bool has_failed = false;
     
     if ( (file = fopen(TESTFILECONTENT, "r")) == NULL )
          goto test_lparchives_get_entry_names_bailout;
     if ( (fd = open(TESTFILE, O_RDONLY)) == -1 )
          goto test_lparchives_get_entry_names_bailout;
     if ( (archive = lparchive_new()) == NULL )
          goto test_lparchives_get_entry_names_bailout;
          lparchive_init(archive);
     if ( (lparchive_open_fd(archive, fd)) == -1)
          goto test_lparchives_get_entry_names_bailout;

     if ( (r = lparchive_get_entry_names(archive)) == NULL )
          goto test_lparchives_get_entry_names_bailout;

     fgets(buf, MAXLEN, file);
     len = atoi(buf);
     for ( i=0; r[i] != NULL; ++i ) {
          fgets(buf, MAXLEN, file);
          buf[strlen(buf)-1] = '\0';
          if (strcmp(buf, r[i]) != 0)
               has_failed = true;
     }
     if ( has_failed || len != i )
               goto test_lparchives_get_entry_names_bailout;

     for ( i=0; r[i] != NULL; ++i )
          free((char *)r[i]);
     free(r);

     /* clean up */
     lparchive_destroy(archive);
     close(fd);
     fclose(file);

     return 0;

test_lparchives_get_entry_names_bailout:
     if ( archive )
          lparchive_destroy(archive);
     if ( file )
          fclose(file);
     if ( fd && fd != -1)
          close(fd);
     return -1;
}
