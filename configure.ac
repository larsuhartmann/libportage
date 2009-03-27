AC_INIT([libportage], [HEAD], [libportage-bugs@lists.berlios.de])
AC_CONFIG_AUX_DIR(config)
AM_INIT_AUTOMAKE([-Wall -Werror foreign])

# checks for programs
AC_PROG_CC
AC_PROG_LIBTOOL

# checks for compiler features
AC_C_CONST
AC_C_INLINE

# checks for headers
AC_HEADER_STDC
AC_CHECK_HEADERS(errno.h)

#check for functions
AC_REPLACE_FUNCS(strndup)

# check for libarchive and abort if not found
AC_CHECK_HEADERS(archive.h,,AC_MSG_ERROR(archive.h not found!))

DX_INIT_DOXYGEN($PACKAGE_NAME, doxygen.cfg)

# other stuff
AC_CONFIG_HEADERS(config.h)

AC_CONFIG_FILES(Makefile replace/Makefile src/Makefile include/Makefile
                test/Makefile doc/Makefile)

# hack to transform replace lib list into libtool format.
Xsed="sed -e s/^X//"
LTLIBOBJS=`echo X"$LIBOBJS" | \$Xsed -e "s,\. [^.]* ,.lo,g;s,\.[^.]*$,.lo,"`
AC_SUBST(LTLIBOBJS)

AC_OUTPUT
