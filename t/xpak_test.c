#include <liblpxpak.h>
#include <liblputil.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <bzlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <signal.h>
#include <stdbool.h>
#include <string.h>

int
main(void)
{
     char *path = "xpak_test.tbz2";
     char *path1 = "blob1.xpak";
     char *path2 = "blob2.xpak";
     
     lpxpak_t **xpak;
     lpxpak_t **xpak1;
     lpxpak_blob_t *blob1;
     lpxpak_blob_t *blob2;
     char *s;
     bool dump = false;
     int fd;

     /* open the file read-only*/
     if ( (fd = open(path, O_RDONLY)) == -1 )
          return EXIT_FAILURE;
     
     if ( (xpak = lpxpak_parse_fd(fd)) == NULL ) {
          close(fd);
          puts("fehler!");
          return EXIT_FAILURE;
     }
     if ( strcmp(xpak[0]->name, "USE") != 0 ) {
          lpxpak_destroy(xpak);
          puts("fehler!");
          return EXIT_FAILURE;
     }
     if ( (blob2 = lpxpak_blob_get_fd(fd)) == NULL ) {
          lpxpak_destroy(xpak);
          puts("fehler!");
          return EXIT_FAILURE;
     }
     close(fd);
     if ( (blob1 = lpxpak_blob_compile(xpak)) == NULL ) {
          lpxpak_destroy(xpak);
          lpxpak_blob_destroy(blob2);
          puts("fehler!");
          return EXIT_FAILURE;
     }
     xpak1 = lpxpak_parse_data(blob1);
     if ( blob1->len == blob2->len) {
          puts("größe identisch!");
          if ( memcmp(blob1->data, blob2->data, blob1->len) == 0 )
               puts("identisch!");
          else {
               puts("blobs nicht identisch!");
               dump = true;
          }
     } else {
          puts("größpe nicht identisch!");
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
     
     lpxpak_destroy(xpak);
     return EXIT_SUCCESS;
}
