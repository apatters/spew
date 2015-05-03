#!/bin/sh

if [ $# -lt 1 ]
then
cat >&2 <<EOF
Usage: `basename $0` <arch>

where <arch> is one of:

   linux-ia32
   linux-ia64
   linux-parisc64
   cygwinnt-ia32
   hpux11-parisc64

EOF
    exit 1
fi

arch=$1
cxxflags=""
ldflags=""
configopts=--enable-static-link
MAKE=make
TAR=tar
TAR_FLAGS=-czf
TAR_EXT=tgz

case $arch in
    linux-ia32)
      libs="-ldl"
      ;;
    linux-ia64)
      libs="-ldl"
      ;;
    linux-parisc64)
      ;;
    cygwinnt-ia32)
      TAR=zip
      TAR_FLAGS=-r 
      TAR_EXT=zip
      ;;
    hpux11-parisc64)
      MAKE=gmake
      TAR=/usr/local/bin/tar
      ldflags="-L/home/andrew/popt-1.7/.libs -lpthread" 
      cxxflags="-I/home/andrew/popt-1.7 -I/usr/local/include/ncurses"
      ;;
    *)
      echo "error: unknown architecture -- use one of linux-ia32, linux-ia64, linux-parisc, winnt, hpux11" >&2
      exit 1
      ;;
esac

# Get version
program_name=`sed -n -e 's/AM_INIT_AUTOMAKE(\(.*\),.*$/\1/p' configure.ac`
age=`sed -n -e 's/.*SPEW_AGE=\([^:space:].*$\)/\1/p' configure.ac`
revision=`sed -n -e 's/.*SPEW_REVISION=\([^:space:].*$\)/\1/p' configure.ac`
current=`sed -n -e 's/.*SPEW_CURRENT=\([^:space:].*$\)/\1/p' configure.ac`

distname="${program_name}-${age}.${revision}.${current}.${arch}"
topdir=`pwd`
builddir=${topdir}/${distname}
installdir=${builddir}/usr/local
tarfile=${distname}.tgz

rm -f ${tarfile}
rm -rf ${builddir}
mkdir -p ${installdir}
if [ -f  Makefile ]
then
  ${MAKE} distclean
fi
LIBS="$libs" CXXFLAGS="$cxxflags" LDFLAGS="$ldflags" ./configure $configopts
${MAKE} clean all
${MAKE} install prefix=${installdir}
( cd ${builddir} && ${TAR} ${TAR_FLAGS} ${topdir}/${distname}.${TAR_EXT} . )
rm -rf ${builddir}

