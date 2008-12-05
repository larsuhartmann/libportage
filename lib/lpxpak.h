/*
 * Copyright (c) 2008, Lars Hartmann
 *
 * Distributed under the Terms of the 3clause BSD License - see LICENSE File
 * if you received this without a copy of the License, please contact the
 * Author (lars<at>chaotika<dot>org) to obtain one
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <arpa/inet.h>

#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
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
lpxpak_parse_data(const void *data, size_t n);

lpxpak_t *
lpxpak_parse_fd(int fd);

lpxpak_t *
lpxpak_parse_file(FILE *file);

lpxpak_t *
lpxpak_parse_path(const char *path);

lpxpak_t *
lpxpak_parse_data(const void *data, size_t n)
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
     lpxpak_t *xpak;
     struct stat xpakstat;
     void *xpakdata;
     void *tmp;
     uint32_t xpakoffset;

     if (fstat(fd, &xpakstat) == -1)
          return NULL;

     if (! S_ISREG(xpakstat.st_mode) ) {
          errno = EINVAL;
          return NULL;
     }

     /* seek to the start of the STOP string */
     lseek(fd, __LP_XPAK_STOP_OFFSET__*-1, SEEK_END);

     /* read in the STOP string */
     tmp = malloc(4);
     read(fd, tmp, 4);

     /* check if the read in string is "STOP" - otherwise this here would be a
      * non valid / non xpak file */
     if (memcmp(tmp, "STOP", 4) != 0) {
          errno = EINVAL;
          return NULL;
     }

     /* seek to the start of the xpak_offset value */
     lseek(fd, __LP_XPAK_OFFSET_OFFSET__*-1, SEEK_END);

     /* read the offset */
     read(fd, &xpakoffset, 4);

     /* convert it from network to local byteorder */
     xpakoffset = ntohl(xpakoffset);

     /* read xpak-blob into xpakstr  */
     xpakdata = malloc(xpakoffset);
     lseek(fd, (off_t)(xpakoffset+__LP_XPAK_OFFSET_OFFSET__)*-1, SEEK_END);
     read(fd, xpakdata, (size_t)xpakoffset);
     
     return lpxpak_parse_data(xpakdata, (size_t)xpakoffset);
}

lpxpak_t *
lpxpak_parse_file(FILE *file)
{
     int fd;

     fd = fileno(file);
     return lpxpak_parse_fd(fd);
}

lpxpak_t *
lpxpak_parse_path(char *path)
{
     int fd;

     fd = open(path, O_RDONLY);

     return lpxpak_parse_fd(fd);
}
