#include <liblpxpak.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>
#include <string.h>

int
main(void)
{
     char *path = "xpak_parse.test.tbz2";
     char *path1 = "blob1.xpak";
     char *path2 = "blob2.xpak";
     
     lpxpak_t **xpak1;
     lpxpak_t **xpak2;
     lpxpak_blob_t *blob1;
     lpxpak_blob_t *blob2;
     bool dump = false;
     int fd;

     printf("%-70s", "lpxpak_parse()");
     /* open the file read-only*/
     if ( (fd = open(path, O_RDONLY)) == -1 )
          return EXIT_FAILURE;
     
     if ( (xpak1 = lpxpak_parse_fd(fd)) == NULL ) {
          close(fd);
          puts("\x1b[1m*fail*\x1b[0m");
          return EXIT_FAILURE;
     }
     if ( strcmp(xpak1[0]->name, "USE" ) != 0 ||
         strcmp(xpak1[22]->name, "CFLAGS") != 0 ) {
          lpxpak_destroy(xpak1);
          puts("\x1b[1m*fail*\x1b[0m");
          return EXIT_FAILURE;
     }
     puts("\x1b[1m*pass*\x1b[0m");
     printf("%-70s", "lpxpak_compile()");
     if ( (blob2 = lpxpak_blob_get_fd(fd)) == NULL ) {
          lpxpak_destroy(xpak1);
          puts("\x1b[1m*fail*\x1b[0m");
          return EXIT_FAILURE;
     }
     close(fd);
     if ( (blob1 = lpxpak_blob_compile(xpak1)) == NULL ) {
          lpxpak_destroy(xpak1);
          lpxpak_blob_destroy(blob2);
          puts("\x1b[1m*fail*\x1b[0m");
          return EXIT_FAILURE;
     }
     xpak2 = lpxpak_parse_data(blob1);
     if ( blob1->len == blob2->len) {
          if ( memcmp(blob1->data, blob2->data, blob1->len) == 0 )
               puts("\x1b[1m*pass*\x1b[0m");
          else {
               puts("\x1b[1m*fail*\x1b[0m");
               dump = true;
          }
     } else {
          puts("\x1b[1m*fail*\x1b[0m");
          dump = true;
     }

     if ( dump ) {
          printf("dumping xpak blobs to %s and %s\n", path1, path2);
          fd = open(path1, O_RDWR);
          lseek(fd, 0, 0);
          write(fd, blob1->data, blob1->len);
          close(fd);
          fd = open(path2, O_WRONLY);
          lseek(fd, 0, 0);
          write(fd, blob2->data, blob2->len);
          close(fd);
     }
     
     lpxpak_destroy(xpak1);
     lpxpak_destroy(xpak2);
     lpxpak_blob_destroy(blob1);
     lpxpak_blob_destroy(blob2);
     return EXIT_SUCCESS;
}
