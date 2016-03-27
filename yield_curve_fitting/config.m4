dnl $Id$
dnl config.m4 for extension curvefitting

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

PHP_ARG_WITH(curvefitting, for curvefitting support,
dnl Make sure that the comment is aligned:
[  --with-curvefitting             Include curvefitting support])

dnl Otherwise use enable:

dnl PHP_ARG_ENABLE(curvefitting, whether to enable curvefitting support,
dnl Make sure that the comment is aligned:
dnl [  --enable-curvefitting           Enable curvefitting support])

if test "$PHP_CURVEFITTING" != "no"; then
  dnl Write more examples of tests here...
  PHP_REQUIRE_CXX()
  PHP_ADD_LIBRARY(stdc++, "", EXTRA_LDFLAGS)
  CPPFILE="php_curvefitting.cpp CurveFitting.cpp mathfunc.cpp"  
  PHP_ADD_INCLUDE(/usr/include/)
 
  dnl # --with-curvefitting -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/curvefitting.h"  # you most likely want to change this
  dnl if test -r $PHP_CURVEFITTING/$SEARCH_FOR; then # path given as parameter
  dnl   CURVEFITTING_DIR=$PHP_CURVEFITTING
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for curvefitting files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       CURVEFITTING_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$CURVEFITTING_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the curvefitting distribution])
  dnl fi

  dnl # --with-curvefitting -> add include path
  dnl PHP_ADD_INCLUDE($CURVEFITTING_DIR/include)

  dnl # --with-curvefitting -> check for lib and symbol presence
  dnl LIBNAME=curvefitting # you may want to change this
  dnl LIBSYMBOL=curvefitting # you most likely want to change this 

  dnl PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $CURVEFITTING_DIR/lib, CURVEFITTING_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_CURVEFITTINGLIB,1,[ ])
  dnl ],[
  dnl   AC_MSG_ERROR([wrong curvefitting lib version or lib not found])
  dnl ],[
  dnl   -L$CURVEFITTING_DIR/lib -lm
  dnl ])
  dnl
  dnl PHP_SUBST(CURVEFITTING_SHARED_LIBADD)

  PHP_NEW_EXTENSION(curvefitting, $CPPFILE, $ext_shared)
fi
