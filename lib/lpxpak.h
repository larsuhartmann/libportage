/*
 * Copyright (c) 2008, Lars Hartmann
 *
 * Distributed under the Terms of the 3clause BSD License - see LICENSE File
 * if you received this without a copy of the License, please contact the
 * Author (lars<at>chaotika<dot>org) to obtain one
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <arpa/inet.h>

#define __LP_XPAK_STOP_OFFSET__         4
#define __LP_XPAK_OFFSET_OFFSET__       8

typedef struct {
     char *name;
     char *xpak;
} lpxpak_t;

lpxpak_t *
lpxpak_parse_data(const void *s, size_t length);

lpxpak_t *
lpxpak_parse_fd(int fd);

lpxpak_t *
lpxpak_parse_file(FILE *file);

lpxpak_t *
lpxpak_parse_path(char *path);

lpxpak_t *
lpxpak_parse_data(const void *s, size_t length)
{
     lpxpak_t *xpak;
     const void *temp = NULL;
     uint32_t count = 0;
     char tmp[9];
     temp = s;
     /* get the first 8 bytes ("XPAKPACK") and check if they read "XPAKPACK"
      * to make sure we have a valid xpak string */
     memcpy(tmp, temp, 8);
     if (! strcmp(tmp, "XPAKPACK") == 0) {
          errno = EINVAL;
          return NULL;
     }
     temp += 8; count += 8;
     /* allocate the needed memory from the Heap */
     xpak = (lpxpak_t *)malloc(sizeof(lpxpak_t));
     return xpak;
}

lpxpak_t *
lpxpak_parse_fd(int fd)
{
     FILE *file;
     lpxpak_t *xpak;

     if ( (file = fdopen(fd, "r")) == NULL)
          return NULL;
     
     xpak = lpxpak_parse_file(file);
     return xpak;
}

lpxpak_t *
lpxpak_parse_file(FILE *file)
{
     lpxpak_t *xpak;
     void *xpakdata;
     void *tmp;
     uint32_t xpakoffset;

     /* seek to the start of the STOP string */
     fseek(file, __LP_XPAK_STOP_OFFSET__*-1, SEEK_END);

     /* read in the STOP string */
     tmp = malloc(4);
     fread(tmp, 1, 4, file);

     /* check if the read in string is "STOP" - otherwise this here would be a
      * non valid / non xpak file */
     if (memcmp(tmp, "STOP", 4) != 0) {
          errno = EINVAL;
          return NULL;
     }

     /* seek to the start of the xpak_offset value */
     fseek(file, __LP_XPAK_OFFSET_OFFSET__*-1, SEEK_END);

     /* read the offset */
     fread(&xpakoffset, 4, 1, file);

     /* convert it from network to local byteorder */
     xpakoffset = ntohl(xpakoffset);

     /* read xpak-blob into xpakstr  */
     xpakdata = malloc(xpakoffset);
     fseek(file, (long)(xpakoffset+__LP_XPAK_OFFSET_OFFSET__)*-1, SEEK_END);
     fread(xpakdata, (size_t)xpakoffset, 1, file);

     xpak = lpxpak_parse_data(xpakdata, (size_t)xpakoffset);
     return xpak;
}

lpxpak_t *
lpxpak_parse_path(char *path)
{
     struct stat ststr;
     lpxpak_t *xpak;
     FILE *xpakfile;

     /* check if path points to a file */
     if (stat(path, &ststr) == -1) 
          return NULL;
     if (! S_ISREG(ststr.st_mode) ) {
          errno = EINVAL;
     }

     /* open the file read-only*/
     if ( (xpakfile = fopen(path, "r")) == NULL)
          return NULL;

     xpak = lpxpak_parse_file(xpakfile);
     return xpak;
}
