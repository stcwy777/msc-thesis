/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2010 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author:                                                              |
  +----------------------------------------------------------------------+
*/

/* $Id: header 297205 2010-03-30 21:09:07Z johannes $ */

#ifndef PHP_CURVEFITTING_H
#define PHP_CURVEFITTING_H

extern zend_module_entry curvefitting_module_entry;
#define phpext_curvefitting_ptr &curvefitting_module_entry

#ifdef PHP_WIN32
#define PHP_CURVEFITTING_API __declspec(dllexport)
#else
#define PHP_CURVEFITTING_API
#endif

extern "C" {
#ifdef ZTS
#include "TSRM.h"
#endif
}

// External head file
#include "CurveFitting.h"

// Custom macro
#define NUM_KNOTS 6
#define NUM_LOG	2
#define NUM_INTERVAL 10

PHP_MINIT_FUNCTION(curvefitting);
PHP_MSHUTDOWN_FUNCTION(curvefitting);
PHP_RINIT_FUNCTION(curvefitting);
PHP_RSHUTDOWN_FUNCTION(curvefitting);
PHP_MINFO_FUNCTION(curvefitting);

PHP_FUNCTION(confirm_curvefitting_compiled);	/* For testing, remove later. */
PHP_FUNCTION(yield_curve_fitting);

/* 
  	Declare any global variables you may need between the BEGIN
	and END macros here:     

ZEND_BEGIN_MODULE_GLOBALS(curvefitting)
	long  global_value;
	char *global_string;
ZEND_END_MODULE_GLOBALS(curvefitting)
*/

/* In every utility function you add that needs to use variables 
   in php_curvefitting_globals, call TSRMLS_FETCH(); after declaring other 
   variables used by that function, or better yet, pass in TSRMLS_CC
   after the last function argument and declare your utility function
   with TSRMLS_DC after the last declared argument.  Always refer to
   the globals in your function as CURVEFITTING_G(variable).  You are 
   encouraged to rename these macros something shorter, see
   examples in any other php module directory.
*/

#ifdef ZTS
#define CURVEFITTING_G(v) TSRMG(curvefitting_globals_id, zend_curvefitting_globals *, v)
#else
#define CURVEFITTING_G(v) (curvefitting_globals.v)
#endif

#endif	/* PHP_CURVEFITTING_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
