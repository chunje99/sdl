#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.60])
AC_INIT([base], [1.0], [chunje99@gmail.com])
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE([foreign -Wall -Werror])

AC_CONFIG_MACRO_DIRS([m4])

AM_PROG_AR


# Checks for programs.
LT_INIT
AC_PROG_CXX
AC_PROG_RANLIB
AC_PROG_LIBTOOL
#AC_LIBTOOL_DLOPEN


AC_ARG_ENABLE([debug],
  [AS_HELP_STRING([--enable-debug],
                  [whether to include debug symbols (default is no)])],
  [enable_debug=$enableval],
  [enable_debug=no]
)

#  dnl Remove all optimization flags from CFLAGS
changequote({,})
CFLAGS=`echo "$CFLAGS" | $SED -e 's/-O[0-9s]*//g'`
CXXFLAGS=`echo "$CXXFLAGS" | $SED -e 's/-O[0-9s]*//g'`

CFLAGS=`echo "$CFLAGS" | $SED -e 's/-g[0-9]*//g'`
CXXFLAGS=`echo "$CXXFLAGS" | $SED -e 's/-g[0-9]*//g'`
changequote([,])
if test "x$enable_debug" = xyes; then
  CFLAGS="$CFLAGS -g -O0 -DDEBUG"
  CXXFLAGS="$CXXFLAGS -g -O0 -DDEBUG"
  else
  CFLAGS="$CFLAGS  -O2 -DNDEBUG"
  CXXFLAGS="$CXXFLAGS -O2 -DNDEBUG"
fi

# Checks for libraries.

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.
# AC_CANONICAL_HOST is needed to access the 'host_os' variable    
AC_CANONICAL_HOST
# Detect the target system
case "${host_os}" in
    linux*)
        AC_DEFINE([OS_LINUX], ["LINUX"], ["OS TYPE"])
        ;;
    cygwin*|mingw*)
        AC_DEFINE([OS_WINDOWS], ["WINDOWS"], ["OS TYPE"])
        ;;
    darwin*)
        AC_DEFINE([OS_MAC], ["MAC"], ["OS TYPE"])
        ;;
    *)
        AC_MSG_ERROR(["OS $host_os is not supported"])
        ;;
esac

## Pass the conditionals to automake
#AM_CONDITIONAL([LINUX], [test "$build_linux" = "yes"])
#AM_CONDITIONAL([WINDOWS], [test "$build_windows" = "yes"])
#AM_CONDITIONAL([OSX], [test "$build_mac" = "yes"])

PKG_CONFIG
# Checks for library functions.
PKG_CHECK_MODULES([SDL], [sdl])
PKG_CHECK_MODULES([SDL2], [sdl2, sdl2_image, sdl2_ttf], [ ] ,[ 
   PKG_CHECK_MODULES([SDL2], [sdl2, SDL2_image, SDL2_ttf])
])
PKG_CHECK_MODULES([FFMPEG], [libavcodec libavformat libavutil libswscale libswresample libavdevice])
PKG_CHECK_MODULES([GLOG], [libglog])
PKG_CHECK_MODULES([GFLAGS], [gflags])

AC_CONFIG_FILES([Makefile
                 ])
AC_OUTPUT
