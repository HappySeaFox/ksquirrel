AC_INIT([ksquirrel], [0.6.2], [ksquirrel@tut.by])
AC_CONFIG_SRCDIR([ksquirrel/main.cpp])
AC_GNU_SOURCE
#AM_INIT_AUTOMAKE([foreign])
#AC_CONFIG_HEADER([config.h])
AC_CONFIG_AUX_DIR([admin])

AM_INIT_AUTOMAKE(ksquirrel, 0.6.2)
AM_CONFIG_HEADER([config.h])

unset CDPATH

#AC_CANONICAL_SYSTEM
#AC_ARG_PROGRAM

KDE_SET_PREFIX

sq_ksmall="yes"

AC_ENABLE_SHARED(yes)
AC_ENABLE_STATIC(no)
KDE_PROG_LIBTOOL

dnl for NLS support. Call them in this order!
dnl WITH_NLS is for the po files
AM_KDE_WITH_NLS

KDE_USE_QT(3)
AC_PATH_KDE

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_RANLIB
AM_PROG_AS
AC_PATH_PROG([PERL], [perl])
AC_CHECK_HEADER([math.h], [], AC_MSG_ERROR("ksquirrel needs math.h"))

# Checks for header files.
AC_PATH_X
AC_PATH_XTRA
AC_HEADER_STDC
AC_CHECK_HEADERS([limits.h stdlib.h unistd.h stdio.h], [], AC_MSG_ERROR("Some headers are missing"))
AC_CHECK_HEADER([setjmp.h], [], AC_MSG_ERROR("ksquirrel needs setjmp.h"))

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_TYPE_SIZE_T

PKG_CHECK_MODULES([ksquirrellibs], ksquirrellibs, [

        __cppflags=$CPPFLAGS

        CXXFLAGS="$ksquirrellibs_CFLAGS $CXXFLAGS"
        CPPFLAGS="$ksquirrellibs_CFLAGS $CPPFLAGS"

        AC_SUBST(ksquirrellibs_LIBS)

        AC_LANG_SAVE
        AC_LANG_CPLUSPLUS

        AC_CHECK_HEADERS([ksquirrel-libs/fileio.h], [

#                                AC_TRY_RUN([
#include <fileio.h>

#int main()
#{
#    ifstreamK i;
#
#    i.open("1", ios::in);
#
#    i.close();
#
#    return 0;
#}
#
#], [], [AC_MSG_ERROR([Ksquirrel-libs is not installed. Please install ksquirrel-libs first.])], $ksquirrellibs_LIBS)
                            ],
                            [AC_MSG_ERROR([Ksquirrel-libs development files are not installed. Please install them first.])])


        # restore language & CPPFLAGS
        CPPFLAGS=${__cppflags}
        AC_LANG_RESTORE
                            ],
                [AC_MSG_ERROR([Ksquirrel-libs development file (.pc) is not installed. Please install ksquirrel-libs first.])]
)

KDE_CHECK_HEADER(konq_operations.h, [], AC_MSG_ERROR("Header file konq_operations.h is missing. Please install kdebase-devel package."))

#AC_CHECK_LIB([kio], [KFileIconView], [], AC_MSG_ERROR("KIO library is corrupted!"))

__cflags=$CFLAGS

CFLAGS="$X_INCLUDES $CFLAGS"
LDFLAGS="$X_PRE_LIBS -lX11 $X_LDFLAGS $X_EXTRA_LIBS $LDFLAGS"

AC_CHECK_HEADERS([GL/gl.h GL/glu.h], [], AC_MSG_ERROR("ksquirrel needs OpenGL (GL/gl.h, ...) and GLU (GL/glu.h) headers to be installed."))

SQ_GL_ERROR="OpenGL library is corrupted or not installed"

AC_CHECK_LIB([GL], [glBegin], [], AC_MSG_ERROR($SQ_GL_ERROR))

AC_CHECK_FUNC([glEnd], [], AC_MSG_ERROR($SQ_GL_ERROR))
AC_CHECK_FUNC([glGenTextures], [], AC_MSG_ERROR($SQ_GL_ERROR))
AC_CHECK_FUNC([glBindTexture], [], AC_MSG_ERROR($SQ_GL_ERROR))
AC_CHECK_FUNC([glTexImage2D], [], AC_MSG_ERROR($SQ_GL_ERROR))
AC_CHECK_FUNC([glColor3f], [], AC_MSG_ERROR($SQ_GL_ERROR))
AC_CHECK_FUNC([glClearColor], [], AC_MSG_ERROR($SQ_GL_ERROR))
AC_CHECK_FUNC([glClearDepth], [], AC_MSG_ERROR($SQ_GL_ERROR))
AC_CHECK_FUNC([glClear], [], AC_MSG_ERROR($SQ_GL_ERROR))
AC_CHECK_FUNC([glEnable], [], AC_MSG_ERROR($SQ_GL_ERROR))
AC_CHECK_FUNC([glDisable], [], AC_MSG_ERROR($SQ_GL_ERROR))
AC_CHECK_FUNC([glBlendFunc], [], AC_MSG_ERROR($SQ_GL_ERROR))
AC_CHECK_FUNC([glShadeModel], [], AC_MSG_ERROR($SQ_GL_ERROR))
AC_CHECK_FUNC([glMatrixMode], [], AC_MSG_ERROR($SQ_GL_ERROR))
AC_CHECK_FUNC([glViewport], [], AC_MSG_ERROR($SQ_GL_ERROR))
AC_CHECK_FUNC([glLoadIdentity], [], AC_MSG_ERROR($SQ_GL_ERROR))
AC_CHECK_FUNC([glOrtho], [], AC_MSG_ERROR($SQ_GL_ERROR))
AC_CHECK_FUNC([glTexParameteri], [], AC_MSG_ERROR($SQ_GL_ERROR))
AC_CHECK_FUNC([glTexCoord2f], [], AC_MSG_ERROR($SQ_GL_ERROR))
AC_CHECK_FUNC([glVertex2f], [], AC_MSG_ERROR($SQ_GL_ERROR))
AC_CHECK_FUNC([glLoadMatrixf], [], AC_MSG_ERROR($SQ_GL_ERROR))
AC_CHECK_FUNC([glMultMatrixd], [], AC_MSG_ERROR($SQ_GL_ERROR))
AC_CHECK_FUNC([glTranslated], [], AC_MSG_ERROR($SQ_GL_ERROR))
AC_CHECK_FUNC([glGetIntegerv], [], AC_MSG_ERROR($SQ_GL_ERROR))

CFLAGS=${__cflags}
CXXFLAGS="$X_INCLUDES $CXXFLAGS"

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_FUNC_REALLOC
AC_FUNC_STAT
#AC_CHECK_FUNCS([mkdir socket sqrt])

AC_CHECK_FUNCS([memcpy memset], [], AC_MSG_ERROR("ksquirrel needs memcpy() and memset() functions!"))

AC_ARG_ENABLE(ksmall,
              [AS_HELP_STRING([--disable-ksmall], [don't compile ksquirrel-small [default=no]])],
              [     case "${enableval}" in
                       no)  sq_ksmall="yes" ;;
                       yes) sq_ksmall="no"  ;;
                       *)   AC_MSG_ERROR([bad value ${enableval} for ksmall param]) ;;
                    esac]
             )

AM_CONDITIONAL([SQ_ENABLE_KSMALL], [test $sq_ksmall = yes])

##################### from gdk-pixbuf-0.22 #########################
#
# Checks to see if we should compile in MMX support (there will be
# a runtime test when the code is actually run to see if it should
# be used - this just checks if we can compile it.)
#
# This code is partially taken from Mesa
#
dnl Let people disable the MMX optimization
AC_ARG_ENABLE(mmx,
              [AS_HELP_STRING([--disable-mmx], [don't use MMX [default=auto]])],
              [     case "${enableval}" in
                       no)  enable_mmx="auto" ;;
                       yes) enable_mmx="no"  ;;
                       *)   AC_MSG_ERROR([bad value ${enableval} for mmx param]) ;;
                    esac]
             )

AC_MSG_CHECKING(for x86 platform)
  case $host_cpu in
    i386|i486|i586|i686|i786|k6|k7)
	use_x86_asm=yes
	;;
    *)
  	use_x86_asm=no
  esac
AC_MSG_RESULT($use_x86_asm)

dnl Are we going to use MMX extensions
use_mmx_asm=no

AC_MSG_CHECKING(compiler support for MMX)

if test x$enable_mmx = xauto ; then
  if test $use_x86_asm = yes; then
    save_ac_ext=$ac_ext
    ac_ext=S

    cp $srcdir/ksquirrel/libpixops/scale_line_22_33_mmx.S conftest.S
    if AC_TRY_EVAL(ac_compile); then
        use_mmx_asm=yes
    fi
    dnl rm -f conftest.[oS]

    ac_ext=$save_ac_ext
  fi

dnl Enforce usage of MMX extensions
elif test x$enable_mmx = xyes ; then
    use_mmx_asm=yes
else
    use_mmx_asm=no
fi

if test $use_mmx_asm = yes; then
  AC_DEFINE([USE_MMX], [], [Use MMX instructions])
  AC_MSG_RESULT(yes)
else
  AC_MSG_RESULT(no)
fi

AM_CONDITIONAL(USE_MMX, test x$use_mmx_asm = xyes)
###################################################

KDE_CREATE_SUBDIRSLIST

AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([ksquirrel/Makefile])
AC_CONFIG_FILES([ksquirrel/libpixops/Makefile])
AC_CONFIG_FILES([ksquirrel/ksquirrel-small/Makefile])
AC_CONFIG_FILES([ksquirrel/imageedit_dlg/Makefile])
AC_CONFIG_FILES([ksquirrel/imageedit_overlay/Makefile])
AC_CONFIG_FILES([ksquirrel/sidebar/Makefile])
AC_CONFIG_FILES([pics/Makefile])
AC_CONFIG_FILES([pics/toolbar/Makefile])
AC_CONFIG_FILES([pics/menu/Makefile])
AC_CONFIG_FILES([pics/imageedit/Makefile])
AC_CONFIG_FILES([pics/imageedit/squirrels/Makefile])
AC_CONFIG_FILES([po/Makefile])
AC_CONFIG_FILES([doc/Makefile])
AC_CONFIG_FILES([doc/en/Makefile])

AC_OUTPUT

echo "
KSquirrel configuration:
-------------------------
  Source code location:     ${srcdir}
  KSquirrel version:        ${VERSION}
  C Compiler:               ${CC}
  C Compiler flags:         ${CFLAGS}
  C++ Compiler:             ${CXX}
  C++ Compiler flags:       ${CXXFLAGS}
  ksquirrel-libs CFLAGS:    $ksquirrellibs_CFLAGS
  ksquirrel-libs LIBS:      $ksquirrellibs_LIBS
  Compile ksquirrel-small:  $sq_ksmall

Installation options:
----------------------
  prefix:                   ${prefix}
  KSquirrel binary:         ${prefix}/bin/${PACKAGE}

  ----------------------------------------------------

"
