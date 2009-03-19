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

#include <xpak.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>
#include <string.h>

int
main(void)
{
     char *path = "03_lpxpak.tbz2";
     char *srcpath;
     
     lpxpak_t *xpak1;
     lpxpak_t *xpak2;
     lpxpak_blob_t *blob1;
     lpxpak_blob_t *blob2;
     int fd;

     if ( (srcpath = getenv("srcdir")) != NULL ) {
          if ( chdir(srcpath) == -1 ) {
               return EXIT_FAILURE;
          }
     }

     /* create initialize xpak objects */
     if ( (xpak1 = lpxpak_create()) == NULL )
          return EXIT_FAILURE;

     if ( (xpak2 = lpxpak_create()) == NULL ) {
          lpxpak_destroy(xpak1);
          return EXIT_FAILURE;
     }

     lpxpak_init(xpak1);
     lpxpak_init(xpak2);
     
     /* open the file read-only*/
     if ( (fd = open(path, O_RDONLY)) == -1 )
          return EXIT_FAILURE;
     
     if ( lpxpak_parse_fd(xpak1, fd) == -1 ) {
          close(fd);
          return EXIT_FAILURE;
     }
     if ( strcmp(xpak1->entries[0].name, "USE" ) != 0 ||
         strcmp(xpak1->entries[22].name, "CFLAGS") != 0 ) {
          lpxpak_destroy(xpak1);
          return EXIT_FAILURE;
     }
     if ( (blob2 = lpxpak_blob_get_fd(fd)) == NULL ) {
          lpxpak_destroy(xpak1);
          return EXIT_FAILURE;
     }
     close(fd);
     if ( (blob1 = lpxpak_blob_compile(xpak1)) == NULL ) {
          lpxpak_destroy(xpak1);
          lpxpak_blob_destroy(blob2);
          return EXIT_FAILURE;
     }
     if ( lpxpak_parse_data(xpak2, blob1) == -1 ) {
          lpxpak_destroy(xpak1);
          lpxpak_blob_destroy(blob2);
          return EXIT_FAILURE;
     }
     if ( blob1->len == blob2->len) {
          if ( memcmp(blob1->data, blob2->data, blob1->len) != 0 ) {
               lpxpak_destroy(xpak1);
               lpxpak_destroy(xpak2);
               lpxpak_blob_destroy(blob1);
               lpxpak_blob_destroy(blob2);
               return EXIT_FAILURE;
          }
     } else {
          return EXIT_FAILURE;
     }

     lpxpak_destroy(xpak1);
     lpxpak_destroy(xpak2);
     lpxpak_blob_destroy(blob1);
     lpxpak_blob_destroy(blob2);
     return EXIT_SUCCESS;
}
