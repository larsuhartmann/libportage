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
     char *path = "02_lpxpak_parse.tbz2";
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
