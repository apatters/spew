#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.57)
AC_INIT(src/spew.cpp)
AC_CONFIG_AUX_DIR(config)
AM_CONFIG_HEADER(config.h)
AM_MAINTAINER_MODE

SPEW_AGE=1
SPEW_REVISION=0
SPEW_CURRENT=6
AC_SUBST(SPEW_AGE)
AC_SUBST(SPEW_REVISION)
AC_SUBST(SPEW_CURRENT)
VERSION=$SPEW_AGE.$SPEW_REVISION.$SPEW_CURRENT

AM_INIT_AUTOMAKE(spew, $VERSION)

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_MAKE_SET
AC_PROG_RANLIB
AC_PROG_LN_S

# Checks for libraries.
AC_CHECK_LIB(popt, poptGetContext, ,AC_MSG_ERROR([PACKAGE] requires popt))
AC_CHECK_LIB(ncurses, wresize, ,AC_MSG_ERROR([PACKAGE] requires ncurses))

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([fcntl.h stdint.h stdlib.h sys/time.h unistd.h paths.h])

# Common defines
AC_DEFINE(_GNU_SOURCE, 1, GNU extensions.)
AC_DEFINE(_REENTRANT, 1, Use reentrant code if available.)

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_TYPE_OFF_T
AC_TYPE_SIZE_T
AC_HEADER_TIME
AC_CHECK_DECL(O_DIRECT,AC_DEFINE(HAVE_DIRECT_IO,[1],Define to 1 if direct I/O is available.),,[#include <fcntl.h>])
AC_CHECK_DECL(TIOCGWINSZ,AC_DEFINE(HAVE_WINSIZE_IOCTL,[1],Define to 1 if there is an ioctl to get the window size.),,[#include <sys/ioctl.h>])
AC_CHECK_DECL(WIOCGETD,AC_DEFINE(HAVE_WINSIZE_IOCTL,[1],Define to 1 if there is an ioctl to get the window size.),,[#include <sys/ioctl.h>])
AC_SYS_LARGEFILE

# Functionality checks.
AC_CHECK_DECL(MNT_MNTTAB,AC_DEFINE(MNTTAB_PATH,MNT_MNTTAB,Path to mtab/mnttab),,[#include <mntent.h>])
AC_CHECK_DECL(_PATH_MOUNTED,AC_DEFINE(MNTTAB_PATH,_PATH_MOUNTED,Path to mnttab),,[#include <paths.h>])
AC_CHECK_DECL(MNTTAB_PATH,mnttab=yes,mnttab=no,)
if test "$mnttab" = yes; then
AC_CHECK_FUNC([getmntent],AC_DEFINE(HAVE_MOUNT_CHECK,[1],Define to 1 if getmntent can be used to get mounted file systems.),,)
fi

# Get typedefs for 32 and 64 bit unsigned integers.
AC_CHECK_SIZEOF(unsigned int)
AC_CHECK_SIZEOF(unsigned long)
AC_CHECK_SIZEOF(unsigned long long)
if test "$ac_cv_sizeof_unsigned_int" -eq 4; then
  AC_DEFINE(UINT32_TYPE, [unsigned int], Unsigned 4-byte integer)
elif test "$ac_cv_sizeof_unsigned_long" -eq 4; then
  AC_DEFINE(UINT32_TYPE, [unsigned long], Unsigned 4-byte integer)
elif test "$ac_cv_sizeof_unsigned_long_long" -eq 4; then
  AC_DEFINE(UINT32_TYPE, [unsigned long long], Unsigned 4-byte integer)
else
  AC_MSG_ERROR([compiler has no 4-byte unsigned integer])
fi
if test "$ac_cv_sizeof_unsigned_int" -eq 8; then
  AC_DEFINE(UINT64_TYPE, [unsigned int], Unsigned 8-byte integer)
elif test "$ac_cv_sizeof_unsigned_long" -eq 8; then
  AC_DEFINE(UINT64_TYPE, [unsigned long], Unsigned 8-byte integer)
elif test "$ac_cv_sizeof_unsigned_long_long" -eq 8; then
  AC_DEFINE(UINT64_TYPE, [unsigned long long], Unsigned 8-byte integer)
else
  AC_MSG_ERROR([compiler has no 8-byte unsigned integer])
fi

# Checks for library functions.
AC_FUNC_MALLOC
AC_TYPE_SIGNAL
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([getpagesize gettimeofday memset strerror strsignal strrchr localtime_r])

# Add --enable-static
AC_ARG_ENABLE(static-link,
	[  --enable-static-link         link spew statically],
	[ case "$enableval" in
          yes) static_spew=yes ;;
          no) static_spew=no;;
          esac
        ],
	static_spew=no)

STATIC_LINK=""
if test "x$static_spew" = "xyes" ; then
  if test "x`uname`" = "xHP-UX" ; then
    STATIC_LINK="-Wl,-a,archive_shared"
  else
    STATIC_LINK="-static"
  fi
fi
AC_SUBST(STATIC_LINK)

CXXFLAGS="$CXXFLAGS -DSYSCONFDIR=${sysconfdir}"

AC_CONFIG_FILES([src/Makefile man/Makefile etc/Makefile])
AC_OUTPUT(Makefile)
