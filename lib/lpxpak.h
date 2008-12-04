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

#define __LP_XPAK_STOP_OFFSET__         4
#define __LP_XPAK_OFFSET_OFFSET__       8

typedef struct {
     char *name;
     char *xpak;
} lpxpak_t;

lpxpak_t *
lpgetxpakstr(const char *s);

lpxpak_t *
lpgetxpakfd(int fd);

lpxpak_t *
lpgetxpakfile(FILE *file);

lpxpak_t *
lpgetxpakpath(char *path);

lpxpak_t *
lpgetxpakstr(const char *s)
{
     lpxpak_t *xpak;
     
     xpak = (lpxpak_t *)malloc(sizeof(lpxpak_t));
     return xpak;
}

lpxpak_t *
lpgetxpakfd(int fd)
{
     lpxpak_t *xpak;
     
     xpak = lpgetxpakfile(NULL);
     return xpak;
}

lpxpak_t *
lpgetxpakfile(FILE *file)
{
     lpxpak_t *xpak;
     char *xpakstr;
     char tmp[5];
     uint32_t xpakoffset;
     int i;
     
     fseek(file, __LP_XPAK_STOP_OFFSET__*-1, SEEK_END);
     fread(tmp, 1, 4, file);
     tmp[4] = '\0';

     if (strcmp(tmp, "STOP") != 0) {
          errno = EINVAL;
          return NULL;
     }

     fseek(file, __LP_XPAK_OFFSET_OFFSET__*-1, SEEK_END);
     fread(&xpakoffset, 4, 1, file);
     xpakoffset = ntohl(xpakoffset);
     printf("%i\n", xpakoffset);

     xpak = lpgetxpakstr(NULL);
     return xpak;
}

lpxpak_t *
lpgetxpakpath(char *path) 
{
     struct stat ststr;
     lpxpak_t *xpak;
     FILE *xpakfile;
     
     if (stat(path, &ststr) == -1) 
          return NULL;
     if (! S_ISREG(ststr.st_mode) ) {
          errno = EINVAL;
     }

     xpakfile = fopen(path, "r");
     
     xpak = lpgetxpakfile(xpakfile);
     return xpak;
}
