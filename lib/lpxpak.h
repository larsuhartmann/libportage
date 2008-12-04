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
