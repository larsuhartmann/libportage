#include <atom.h>

#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


#define MAXLEN 1024

int main(void)
{
     lpatom_t *atom1 = NULL;
     lpatom_t *atom2 = NULL;
     char *s, *fname, *srcpath;
     bool has_failed = false;
     FILE *file;

     if ( (srcpath = getenv("srcdir")) != NULL )
          if ( chdir(srcpath) == -1 )
               return EXIT_FAILURE;
     
     if ( (file = fopen("01_lpatom_parse.txt", "r")) == NULL )
          return EXIT_FAILURE;
          
     if ( (atom1 = lpatom_create()) == NULL )
          return EXIT_FAILURE;
     lpatom_init(atom1);
     if ( (atom2 = lpatom_create()) == NULL )
          return EXIT_FAILURE;
     lpatom_init(atom2);

     if ( (s = malloc(MAXLEN)) == NULL )
          return EXIT_FAILURE;

     while ( fgets(s, MAXLEN, file) != NULL) {
          s[strlen(s)-1] = '\0';
          if ( lpatom_parse(atom1, s) == -1)
               has_failed = true;
          else {
               fname = lpatom_get_fullname(atom1);
               if (strcmp(fname, s) != 0)
                    has_failed = true;
               free(fname);
               if ( lpatom_parse(atom2, s) == -1 )
                    has_failed = true;
               else
                    if ( lpatom_cmp(atom1, atom2) != 0 )
                         has_failed = true;
               
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
