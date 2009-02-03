#include <stdio.h>

#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <liblpatom.h>

#define MAXLEN 1024

int main(void)
{
     lpatom_t *atom1 = NULL;
     lpatom_t *atom2 = NULL;
     char *s, *fname;
     bool has_failed = false;
     FILE *file;

     if ( (atom1 = lpatom_create()) == NULL )
          return EXIT_FAILURE;
     lpatom_init(atom1);
     if ( (atom2 = lpatom_create()) == NULL )
          return EXIT_FAILURE;
     lpatom_init(atom2);

     file = fopen("atom_positive.txt", "r");
     s = malloc(MAXLEN);
     while ( fgets(s, MAXLEN, file) != NULL) {
          s[strlen(s)-1] = '\0';
          printf("%-60s", s);
          if ( lpatom_parse(atom1, s) == -1) {
               puts("\x1b[1m*fail*\x1b[0m");
               has_failed = true;
          }
          else {
               fname = lpatom_get_fullname(atom1);
               if (strcmp(fname, s) != 0) {
                    puts("\x1b[1m*fail*\x1b[0m");
                    has_failed = true;
               } else
                    puts("\x1b[1m*pass*\x1b[0m");
               free(fname);
               printf("%-60s", "lpatom_cmp()");
               if ( lpatom_parse(atom2, s) == -1 ) {
                    puts("\x1b[1m*fail*\x1b[0m");
                    has_failed = true;
               } else {
                    if ( lpatom_cmp(atom1, atom2) != 0 ) {
                         puts("\x1b[1m*fail*\x1b[0m");
                         has_failed = true;
                    } else
                         puts("\x1b[1m*pass*\x1b[0m");
               }
               lpatom_reinit(atom1);
               lpatom_reinit(atom2);
          }
     }
     lpatom_destroy(atom1);
     lpatom_destroy(atom2);
     free(s);
     fclose(file);
     if ( has_failed )
          return EXIT_FAILURE;
     return EXIT_SUCCESS;
}
