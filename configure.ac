dnl Process this file with autoconf to produce a configure script.
AC_PREREQ(2.57)
AC_INIT(beatfreq,0.01)
AC_CONFIG_SRCDIR([main.c])

dnl Checks for programs.
AC_PROG_CC

dnl Checks for libraries.
AC_CHECK_LIB(fftw3, [fftw_execute])
AC_CHECK_LIB(m, [log])
dnl AC_CHECK_LIB(rfftw, [rfftw_one])
AC_CHECK_LIB(ggi, [ggiOpen])

dnl Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS(fcntl.h stdlib.h string.h unistd.h)

dnl Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T
AC_FUNC_MALLOC
AC_CHECK_FUNCS([memmove memset])


dnl AC_CHECK_SIZEOF(fftw_real,[],[#if defined(HAVE_STDDEF_H)
dnl 	                       #include <stddef.h>
dnl 		               #endif
dnl 			       #include <fftw.h>
dnl 			       ]) 
dnl AC_SUBST(SIZEOF_FFTW_REAL,$ac_cv_sizeof_fftw_real)

AC_CHECK_SIZEOF(float)
AC_SUBST(SIZEOF_FLOAT,$ac_cv_sizeof_float)

AC_CHECK_SIZEOF(double)
AC_SUBST(SIZEOF_DOUBLE,$ac_cv_sizeof_double)


dnl Checks for library functions.

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
