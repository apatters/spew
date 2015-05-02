#!/bin/sh

if [ $# -lt 1 ]
then
cat >&2 <<EOF
Usage: `basename $0` <arch>

where <arch> is one of:

   linux-ia32
   linux-ia64
   linux-parisc64
   cygwinxp-ia32
   hpux11-parisc64
   hpux11-ia64

EOF
    exit 1
fi

arch=$1
cxxflags=""
ldflags=""
configopts=--enable-static-link
cxx=g++
cc=gcc
MAKE=make
TAR=tar
TAR_FLAGS=-cf
TAR_EXT=tgz
COMPRESS=gzip
COMPRESS_FLAGS=-c

case $arch in
    linux-ia32)
      libs="-ldl"
      ;;
    linux-ia64)
      libs="-ldl"
      ;;
    linux-parisc64)
      libs="-ldl"
      ;;
    cygwinxp-ia32)
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
     hpux11-ia64)
     cxx=aCC
     cc=aCC
     MAKE=gmake
     TAR=/usr/bin/tar
     COMPRESS=/usr/local/bin/gzip
     COMPRESS_FLAGS=-c
     ldflags="-mt -Wl,-a,archive_shared -L /usr/local/lib/hpux32"
     libs="-lgettextlib -lpopt -lintl -liconv"
     cxxflags="-AA -I/usr/local/include -I/usr/local/include/ncurses -mt"
     ;;
    *)
      echo "error: unknown architecture -- use one of linux-ia32, linux-ia64, linux-parisc64, cygwinxp-ia32, hpux11-parisc64, or hpux11-ia64" >&2
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
LIBS="$libs" CC=${cc} CXX=${cxx} CXXFLAGS="$cxxflags" LD=${ld} LDFLAGS="$ldflags" ./configure $configopts
${MAKE} clean all
${MAKE} install prefix=${installdir}

( cd ${builddir} && ${TAR} ${TAR_FLAGS} - . | ${COMPRESS} ${COMPRESS_FLAGS} > ${topdir}/${distname}.${TAR_EXT} )

rm -rf ${builddir}

