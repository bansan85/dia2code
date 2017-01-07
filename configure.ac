dnl Process this file with autoconf to produce a configure script.
AC_PREREQ([2.68])

AC_CONFIG_HEADER(config.h)

AC_INIT(dia2code, 1.0.0)
AC_CONFIG_SRCDIR([src/main.cpp])
AM_INIT_AUTOMAKE
AC_CONFIG_MACRO_DIR([m4])

LT_INIT

dnl Checks for programs.
AC_PROG_CXX
AN_MAKEVAR([AR], [AC_PROG_AR])
AN_PROGRAM([ar], [AC_PROG_AR])
AC_DEFUN([AC_PROG_AR], [AC_CHECK_TOOL(AR, ar, :)])
AC_PROG_AR
dnl Checks for libraries.


AC_PATH_PROG(XML_CONFIG, xml2-config, no)
if test "$XML_CONFIG" = "no"; then
    AC_MSG_ERROR([Cannot determine configuration of libxml.
Perhaps you forgot to install the package libxml2-devel ?])
else
    AC_SUBST(XML_CFLAGS, `$XML_CONFIG --cflags`)
    AC_SUBST(XML_LIBS, `$XML_CONFIG --libs`)
fi

AC_ARG_WITH(corba,
            AS_HELP_STRING([--with-corba],
                           [build with corba.]))
if test "$with_corba" != "yes"; then
    withcorba=false
else
    withcorba=true
fi
AM_CONDITIONAL(ENABLE_CORBA, test x$withcorba = xtrue)
if test x$withcorba = xtrue; then
    AC_DEFINE(ENABLE_CORBA, 1, [Add Corba functionnality])
fi

dnl Checks for header files.

dnl Checks for typedefs, structures, and compiler characteristics.

dnl Checks for library functions.

AC_OUTPUT(Makefile src/Makefile tests/Makefile)
