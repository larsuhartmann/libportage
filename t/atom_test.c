#include <stdio.h>

#include <sys/types.h>
#include <stdlib.h>
#include <string.h>

#include <liblpatom.h>

#define MAXLEN 1024

int main(void)
{
     lpatom_t *atom = NULL;
     char *s;
     char *fname;
     FILE *file;

     file = fopen("atom_positive.txt", "r");
     s = malloc(MAXLEN);
     while ( fgets(s, MAXLEN, file) != NULL) {
          s[strlen(s)-1] = '\0';
          puts(s);
          if ( (atom = lpatom_parse(s)) == NULL) {
               printf("fehler: %s", s);
               return EXIT_FAILURE;
          }
          fname = lpatom_get_fullname(atom);
          if (strcmp(fname, s) == 0)
               puts("success!");
          else
               puts("error!");
          printf("input: %s\noutput: %s\n", s, fname);
          free(fname);
          lpatom_destroy(atom);
     }
     free(s);
     fclose(file);
     return EXIT_SUCCESS;
}
