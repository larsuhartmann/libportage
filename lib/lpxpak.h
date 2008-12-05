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
lpxpak_parse_data(const void *data, size_t len)
{
     lpxpak_t *xpak;
     uint32_t count = 0;
        
     /* check if the first 8 bytes of the xpak data read "XPAKPACK" and the
      * last 8 bytes of of the data read "XPAKSTOP" to make sure we have a
      * valid xpak */
     if ((memcmp(data, "XPAKPACK", 8) != 0) ||
         (memcmp(data+len-8, "XPAKSTOP", 8 != 0))) {
          errno = EINVAL;
          return NULL;
     }
     count += 8;
        
     /* allocate the needed memory from the Heap */
     xpak = (lpxpak_t *)malloc(sizeof(lpxpak_t));
     data = NULL;
     return xpak;
}

lpxpak_t *
lpxpak_parse_fd(int fd)
{
     struct stat xpakstat;
     void *xpakdata;
     void *tmp;
     uint32_t xpakoffset;
     lpxpak_t *xpak;
        
     if (fstat(fd, &xpakstat) == -1)
          return NULL;
        
     if (! S_ISREG(xpakstat.st_mode) ) {
          errno = EINVAL;
          return NULL;
     }

     /* seek to the start of the STOP string */
     lseek(fd, __LP_XPAK_STOP_OFFSET__*-1, SEEK_END);
        
     /* allocate 4bytes from the heap, read in 4bytes, check if the read in
      * data is "STOP"(encoded as an ASCII String) - otherwise this would be
      * an invalid xpak and finally free that piece of memory */
     tmp = malloc(4);
     read(fd, tmp, 4);
     if (memcmp(tmp, "STOP", 4) != 0) {
          free(tmp);
          errno = EINVAL;
          return NULL;
     }
     free(tmp);
        
     /* seek to the start of the xpak_offset value */
     lseek(fd, __LP_XPAK_OFFSET_OFFSET__*-1, SEEK_END);
        
     /* read the offset */
     read(fd, &xpakoffset, 4);
        
     /* convert it from network to local byteorder */
     xpakoffset = ntohl(xpakoffset);
        
     /* allocate <xpakoffset> bytes of data and read the xpak_blob in. */
     xpakdata = malloc(xpakoffset);
     lseek(fd, (off_t)(xpakoffset+__LP_XPAK_OFFSET_OFFSET__)*-1, SEEK_END);
     read(fd, xpakdata, (size_t)xpakoffset);
        
     xpak = lpxpak_parse_data(xpakdata, (size_t)xpakoffset);

     free(xpakdata);
     return xpak;
}

lpxpak_t *
lpxpak_parse_file(FILE *file)
{
     int fd;

     fd = fileno(file);
     return lpxpak_parse_fd(fd);
}

lpxpak_t *
lpxpak_parse_path(const char *path)
{
     int fd;

     fd = open(path, O_RDONLY);
     return lpxpak_parse_fd(fd);
}
