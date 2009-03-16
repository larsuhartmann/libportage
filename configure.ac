AC_INIT([libportage],[HEAD],[lars@chaotika.org])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])

# checks for programs
AC_PROG_CC
AC_PROG_LIBTOOL

# checks for compiler features
AC_C_CONST
AC_C_INLINE

# checks for headers
AC_HEADER_STDC
AC_CHECK_HEADERS([errno.h stdlib.h string.h memory.h unistd.h])

# other stuff
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile src/Makefile include/Makefile t/Makefile
                 doc/Makefile])
AC_OUTPUT
