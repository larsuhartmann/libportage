AC_INIT([libportage],[HEAD],[lars@chaotika.org])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])

# checks for programs
AC_PROG_CC
AC_PROG_LIBTOOL

# checks for headers
AC_HEADER_STDC
AC_CHECK_HEADERS([errno.h stdlib.h string.h memory.h unistd.h stdint.h \
regex.h sys/types.h sys/stat.h arpa/inet.h])

# check for typedefs, structs and compiler characteristics
AC_C_CONST
AC_TYPE_SIZE_T
AC_TYPE_UINT32_T

# check for functions

AC_CHECK_FUNCS([regcomp regexec regfree realloc malloc free strlen strtok \
strcmp memcpy strdup strncpy memcmp sprintf fstat open close read lseek ntohl \
htonl])

# other stuff
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile src/Makefile include/Makefile t/Makefile])
AC_OUTPUT
