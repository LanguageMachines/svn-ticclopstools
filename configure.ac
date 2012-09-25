#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.60])
AC_INIT([ticclopstools], [0.1], [to-be-decided@ergens.nl])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE

AC_CONFIG_SRCDIR([src/anaserv.cxx])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_RANLIB
# libtool stuff
# unfortunately, a lot of old libtools hang around.
# so LT_INIT and LT_PREREQ cannot be used for now
#LT_PREREQ([2.2.0])
#LT_INIT
AC_PROG_LIBTOOL
AC_SUBST([LIBTOOL_DEPS])

# Checks for libraries.

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.
ACX_PTHREAD([],[AC_MSG_NOTICE([no pthread support found])])
if test x"$acx_pthread_ok" = xyes; then
   LIBS="$PTHREAD_LIBS $LIBS" \
   CXXFLAGS="$CXXFLAGS $PTHREAD_CFLAGS"
fi

# ugly hack when PKG_CONFIG_PATH isn't defined.
# couldn't get it to work otherwise
if test "x$PKG_CONFIG_PATH" = x; then
     export PKG_CONFIG_PATH=""
fi
PKG_PROG_PKG_CONFIG


AC_ARG_WITH(timbl,
       [  --with-timbl=DIR       use timbl installed in <DIR>; 
               note that you can install timbl in a non-default directory with
               ./configure --prefix=<DIR> in the timbl installation directory],
       [PKG_CONFIG_PATH="$PKG_CONFIG_PATH:$withval/lib/pkgconfig"],
       [PKG_CONFIG_PATH="$PKG_CONFIG_PATH:$prefix/lib/pkgconfig"])
#  AC_MSG_NOTICE( [pkg-config search path: $PKG_CONFIG_PATH] )
PKG_CHECK_MODULES([timbl], [timbl >= 6.4.2] )
CXXFLAGS="$CXXFLAGS $timbl_CFLAGS"
LIBS="$timbl_LIBS $LIBS"

AC_ARG_WITH(timblserver,
       [  --with-timblserver=DIR       use timblserver installed in <DIR>; 
               note that you can install timbl in a non-default directory with
               ./configure --prefix=<DIR> in the timbl installation directory],
       [PKG_CONFIG_PATH="$PKG_CONFIG_PATH:$withval/lib/pkgconfig"])
#  AC_MSG_NOTICE( [pkg-config search path: $PKG_CONFIG_PATH] )
PKG_CHECK_MODULES([timblserver], [timblserver >= 1.4] )
CXXFLAGS="$CXXFLAGS $timblserver_CFLAGS"
LIBS="$timblserver_LIBS $LIBS"


AC_ARG_WITH(ticcutils,
       [  --with-ticcutils=DIR       use ticcutils installed in <DIR>; 
               note that you can install ticcutils in a non-default directory w
ith
               ./configure --prefix=<DIR> in the ticcutils installation directo
ry],
       [PKG_CONFIG_PATH="$PKG_CONFIG_PATH:$withval/lib/pkgconfig"],
       [PKG_CONFIG_PATH="$PKG_CONFIG_PATH:$prefix/lib/pkgconfig"])
#  AC_MSG_NOTICE( [pkg-config search path: $PKG_CONFIG_PATH] )
PKG_CHECK_MODULES([ticcutils], [ticcutils >= 0.1] )
CXXFLAGS="$CXXFLAGS $ticcutils_CFLAGS"
LIBS="$ticcutils_LIBS $LIBS"

AC_CONFIG_FILES([Makefile
                 include/Makefile
                 include/ticclops/Makefile
                 src/Makefile
                 docs/Makefile])
AC_OUTPUT