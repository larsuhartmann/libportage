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

     file = fopen("atom_positive.txt", "r");
     s = malloc(MAXLEN);
     while ( fgets(s, MAXLEN, file) != NULL) {
          s[strlen(s)-1] = '\0';
          printf("%-60s", s);
          if ( (atom1 = lpatom_parse(s)) == NULL) {
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
          }
          printf("%-60s", "lpatom_cmp()");
          if ( (atom2 = lpatom_parse(s)) == NULL ) {
               puts("\x1b[1m*fail*\x1b[0m");
               has_failed = true;
          }
          if ( lpatom_cmp(atom1, atom2) != 0 ) {
               puts("\x1b[1m*fail*\x1b[0m");
               has_dfailed = true;
          } else
               puts("\x1b[1m*pass*\x1b[0m");
          lpatom_destroy(atom1);
     }
     free(s);
     fclose(file);
     if ( has_failed )
          return EXIT_FAILURE;
     return EXIT_SUCCESS;
}
