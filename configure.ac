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

CXXFLAGS="$CXXFLAGS -std=c++11"

# Checks for libraries.

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

PKG_CONFIG
# Checks for library functions.
PKG_CHECK_MODULES([SDL], [sdl])
PKG_CHECK_MODULES([SDL2], [sdl2, sdl2_image], [ ] ,[ 
   PKG_CHECK_MODULES([SDL2], [sdl2, SDL2_image])
])
PKG_CHECK_MODULES([FFMPEG], [libavcodec libavformat libavutil libswscale libswresample])

AC_CONFIG_FILES([Makefile
                 ])
AC_OUTPUT
