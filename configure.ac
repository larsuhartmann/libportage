AC_INIT([libportage], [HEAD], [libportage-bugs@lists.berlios.de])
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

# doxygen stuff
DX_HTML_FEATURE(OFF)
DX_CHM_FEATURE(OFF)
DX_CHI_FEATURE(OFF)
DX_MAN_FEATURE(OFF)
DX_RTF_FEATURE(OFF)
DX_XML_FEATURE(OFF)
DX_PDF_FEATURE(OFF)
DX_PS_FEATURE(OFF)

DX_INIT_DOXYGEN($PACKAGE_NAME, doxygen.cfg)

# other stuff
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile src/Makefile include/Makefile t/Makefile
                 doc/Makefile])
AC_OUTPUT
