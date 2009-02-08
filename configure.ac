AC_INIT([libportage], [0.1alpha], [lars@chaotika.org])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])

# checks for programs
AC_PROG_CC
AC_PROG_LIBTOOL

# checks for headers
AC_HEADER_STDC
AC_CHECK_HEADERS([errno.h stdlib.h string.h unistd.h stdint.h regex.h])

# check for typedefs, structs and compiler characteristics
AC_C_CONST
AC_TYPE_SIZE_T
AC_TYPE_UINT32_T

# other stuff
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile src/Makefile include/Makefile])
AC_OUTPUT
