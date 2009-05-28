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
AC_REPLACE_FUNCS(strndup stpcpy)

# check for libarchive and abort if not found
AC_CHECK_HEADERS(archive.h,,AC_MSG_ERROR(archive.h not found!))
AC_CHECK_LIB(archive,main,,AC_MSG_ERROR(libarchive not found!))

DX_INIT_DOXYGEN($PACKAGE_NAME, doxygen.cfg)

# hack to get valgrind support via --enable-valgrind
USE_VALGRIND=false
AC_ARG_ENABLE(valgrind,
    [  --enable-valgrind       use valgrind when running unit tests. ],
    USE_VALGRIND=true
)
if [[ "$USE_VALGRIND" = "true" ]]; then
    AC_CHECK_PROG(HAVE_VALGRIND, valgrind, yes, no)
    if [[ "$HAVE_VALGRIND" = "no" ]]; then
        AC_MSG_ERROR([Valgrind not found in PATH. ])
    fi
fi
AM_CONDITIONAL(USE_VALGRIND, $USE_VALGRIND)

# other stuff
AC_CONFIG_HEADERS(config.h)

AC_CONFIG_FILES(Makefile replace/Makefile src/Makefile include/Makefile
                test/Makefile doc/Makefile)

# hack to transform replace lib list into libtool format.
Xsed="sed -e s/^X//"
LTLIBOBJS=`echo X"$LIBOBJS" | \$Xsed -e "s,\. [^.]* ,.lo,g;s,\.[^.]*$,.lo,"`
AC_SUBST(LTLIBOBJS)

AC_OUTPUT
