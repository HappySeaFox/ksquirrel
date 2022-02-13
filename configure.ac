AC_INIT([ksquirrel], [0.6.0-pre4], [ksquirrel@tut.by])
AC_CONFIG_SRCDIR([ksquirrel/main.cpp])
AC_GNU_SOURCE
#AM_INIT_AUTOMAKE([foreign])
#AC_CONFIG_HEADER([config.h])
AC_CONFIG_AUX_DIR([admin])

AM_INIT_AUTOMAKE(ksquirrel, 0.6.0-pre5)
AM_CONFIG_HEADER([config.h])

unset CDPATH

#AC_CANONICAL_SYSTEM
#AC_ARG_PROGRAM

KDE_SET_PREFIX

sq_ogl="yes"

AC_ARG_ENABLE(opengl,
              [AS_HELP_STRING([--disable-opengl], [don't use OpenGL  [default=no]])],
              [     case "${enableval}" in
                       yes) sq_ogl="yes" ;;
                       no)  sq_ogl="no"; AC_DEFINE([SQ_DISABLE_OPENGL], [], [Disable OpenGL support])  ;;
                       *)   AC_MSG_ERROR([bad value ${enableval} for opengl param]) ;;
                    esac]
             )


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
AC_PATH_PROG([PERL], [perl])

AC_CHECK_HEADER([math.h], [], AC_MSG_ERROR("ksquirrel needs math.h"))

#AC_CHECK_LIB([kio], [KFileIconView], [], AC_MSG_ERROR("KIO library is corrupted!"))

# Checks for header files.
AC_PATH_X
AC_PATH_XTRA
AC_HEADER_STDC
AC_CHECK_HEADERS([limits.h stdlib.h unistd.h stdio.h], [], AC_MSG_ERROR("Some headers are missing"))
AC_CHECK_HEADER([setjmp.h], [], AC_MSG_ERROR("ksquirrel needs setjmp.h"))

CFLAGS="$X_INCLUDES $CFLAGS"
LDFLAGS="$X_PRE_LIBS -lX11 $X_LDFLAGS $X_EXTRA_LIBS $LDFLAGS"

if test "x$sq_ogl" = "xyes"; then

AC_CHECK_HEADERS([GL/gl.h GL/glu.h], [], AC_MSG_ERROR("ksquirrel needs OpenGL (GL/gl.h) and GLU (GL/glu.h) headers to be installed."))

SQ_GL_ERROR="OpenGL library is corrupted or not installed."

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

fi

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_FUNC_REALLOC
AC_FUNC_STAT
#AC_CHECK_FUNCS([mkdir socket sqrt])

AC_CHECK_FUNCS([memcpy memset], [], AC_MSG_ERROR("ksquirrel needs memcpy() and memset() functions!"))

KDE_CREATE_SUBDIRSLIST

AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([ksquirrel/Makefile])
AC_CONFIG_FILES([ksquirrel/imageedit_dlg/Makefile])
AC_CONFIG_FILES([ksquirrel/imageedit_overlay/Makefile])
AC_CONFIG_FILES([desktop/Makefile])
AC_CONFIG_FILES([pics/Makefile])
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
  Source code location: ${srcdir}
  KSquirrel version:    ${VERSION}
  C Compiler:           ${CC}
  C Compiler flags:     ${CFLAGS}
  C++ Compiler:         ${CXX}
  C++ Compiler flags:   ${CXXFLAGS}
  
Installation options:
----------------------
  prefix:               ${prefix}
  KSquirrel binary:     ${prefix}/bin/${PACKAGE}
  Enable OpenGL:        $sq_ogl
"
