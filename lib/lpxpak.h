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
     uint32_t xpakoffset, test;
     int ret;

     /* seek to the start of the STOP string */
     fseek(file, __LP_XPAK_STOP_OFFSET__*-1, SEEK_END);
     
#ifdef Debug
     printf("position after seek: %ld\n", ftell(file));
#endif
     
     /* read in the STOP string */
     fread(tmp, 1, 4, file);
     tmp[4] = '\0';

#ifdef Debug
     printf("position after read: %ld\n", ftell(file));
#endif
     
#ifdef Debug
     puts(tmp);
#endif
     
     /* check if the read in string is "STOP" - otherwise this here would be a
      * non valid / non xpak file */
     if (strcmp(tmp, "STOP") != 0) {
          errno = EINVAL;
          return NULL;
     }

     /* seek to the start of the xpak_offset value */
     fseek(file, __LP_XPAK_OFFSET_OFFSET__*-1, SEEK_END);
     /* read the offset */
     
#ifdef Debug
     printf("position after seek: %ld\n", ftell(file));
#endif
     
     fread(&xpakoffset, 4, 1, file);
     
#ifdef Debug
     printf("position after read: %ld\n", ftell(file));
#endif

     /* convert it from network to local byteorder */
     xpakoffset = ntohl(xpakoffset);
     
#ifdef Debug
     printf("%u\n", xpakoffset);
#endif
     
     xpakstr = (char *)malloc(xpakoffset+1);
     fseek(file, (long)xpakoffset*-1, SEEK_END);
     
#ifdef Debug
     printf("position after seek: %ld\n", ftell(file));
#endif
     ret = fread(&test, 1, 4, file);
     printf("returnwert von fread: %u\n", ret);

#ifdef Debug
     printf("position after read: %ld\n", ftell(file));
#endif

     test = ntohl(test);
     printf("%u\n", test);

     xpak = lpgetxpakstr(NULL);
     return xpak;
}

lpxpak_t *
lpgetxpakpath(char *path) 
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
     xpakfile = fopen(path, "r");

     xpak = lpgetxpakfile(xpakfile);
     return xpak;
}
