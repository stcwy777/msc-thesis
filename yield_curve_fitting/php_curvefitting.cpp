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


extern "C" {
	#ifdef HAVE_CONFIG_H
		#include "config.h"
	#endif
	#include "php.h"
	#include "php_ini.h"
	#include "ext/standard/info.h"
}

#include "php_curvefitting.h"

/* If you declare any globals in php_curvefitting.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(curvefitting)
*/

/* True global resources - no need for thread safety here */
static int le_curvefitting;

/* {{{ curvefitting_functions[]
 *
 * Every user visible function must have an entry in curvefitting_functions[].
 */
zend_function_entry curvefitting_functions[] = {
	PHP_FE(confirm_curvefitting_compiled, NULL)		/* For testing, remove later. */
	PHP_FE(yield_curve_fitting, NULL)
	{NULL, NULL, NULL}	/* Must be the last line in curvefitting_functions[] */
};
/* }}} */

/* {{{ curvefitting_module_entry
 */
zend_module_entry curvefitting_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"curvefitting",
	curvefitting_functions,
	PHP_MINIT(curvefitting),
	PHP_MSHUTDOWN(curvefitting),
	PHP_RINIT(curvefitting),		/* Replace with NULL if there's nothing to do at request start */
	PHP_RSHUTDOWN(curvefitting),	/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(curvefitting),
#if ZEND_MODULE_API_NO >= 20010901
	"0.1", /* Replace with version number for your extension */
#endif
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_CURVEFITTING
	BEGIN_EXTERN_C()
	ZEND_GET_MODULE(curvefitting)
    END_EXTERN_C()
#endif

/* {{{ PHP_INI
 */
/* Remove comments and fill if you need to have entries in php.ini
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("curvefitting.global_value",      "42", PHP_INI_ALL, OnUpdateLong, global_value, zend_curvefitting_globals, curvefitting_globals)
    STD_PHP_INI_ENTRY("curvefitting.global_string", "foobar", PHP_INI_ALL, OnUpdateString, global_string, zend_curvefitting_globals, curvefitting_globals)
PHP_INI_END()
*/
/* }}} */

/* {{{ php_curvefitting_init_globals
 */
/* Uncomment this function if you have INI entries
static void php_curvefitting_init_globals(zend_curvefitting_globals *curvefitting_globals)
{
	curvefitting_globals->global_value = 0;
	curvefitting_globals->global_string = NULL;
}
*/
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(curvefitting)
{
	/* If you have INI entries, uncomment these lines 
	REGISTER_INI_ENTRIES();
	*/
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(curvefitting)
{
	/* uncomment this line if you have INI entries
	UNREGISTER_INI_ENTRIES();
	*/
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(curvefitting)
{
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(curvefitting)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(curvefitting)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "curvefitting support", "enabled");
	php_info_print_table_end();

	/* Remove comments if you have entries in php.ini
	DISPLAY_INI_ENTRIES();
	*/
}
/* }}} */


/* Remove the following function when you have succesfully modified config.m4
   so that your module can be compiled into PHP, it exists only for testing
   purposes. */

/* Every user-visible function in PHP should document itself in the source */
/* {{{ proto string confirm_curvefitting_compiled(string arg)
   Return a string to confirm that the module is compiled in */
PHP_FUNCTION(confirm_curvefitting_compiled)
{
	char *arg = NULL;
	int arg_len, len;
	char *strg;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &arg, &arg_len) == FAILURE) {
		return;
	}

	len = spprintf(&strg, 0, "Congratulations! You have successfully modified ext/%.78s/config.m4. Module %.78s is now compiled into PHP.", "curvefitting", arg);

	RETURN_STRINGL(strg, len, 0);
}
/* }}} */
/* The previous line is meant for vim and emacs, so it can correctly fold and 
   unfold functions in source code. See the corresponding marks just before 
   function definition, where the functions purpose is also documented. Please 
   follow this convention for t// Custom Marco
*/

/*
  +----------------------------------------------------------------------+
  | Custom Function                                                      |
  +----------------------------------------------------------------------+
*/

PHP_FUNCTION(yield_curve_fitting)
{
	CurveFitting fitHandler;	// Curve fitting handler	
	double*	coVars;				// Co-variables as linear regression input
	double* resVars;			// Response variables as linear regression input
	size_t	inputSize;			// Size of input array
	double* paramVect;			// Fitted parameter vector	
	long	numKnots;			// # of knots
	zval* data_x;				// Input array of co-variable from php
	zval* data_y;				// Input array of response variabale from php
	zval* data_knots;			// Input array of Knots value
	zval* result_array;			// Result paramters of linear regression for php
	zval**	data_temp;			// Temp variable for restoring data

	// B-spline knots determined by ad hoc
	//double knots[NUM_KNOTS] = {0 ,1, 2, 3, 4, 5, 6, 7, 9, 25};
	//double knots[NUM_KNOTS] = {-1, 0, 1, 4, 8, 26};
	double* knots;

	// Read in input parameters
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zzz", &data_x, &data_y, &data_knots) == FAILURE) 
	{  
		// Automatically insert false
		return;  
	}

	// Read in array size
	inputSize = zend_hash_num_elements(Z_ARRVAL_P(data_x));
	numKnots = zend_hash_num_elements(Z_ARRVAL_P(data_knots));
	//RETURN_LONG(numKnots);
	// Allocate space for linear regression input
	coVars = new double [inputSize];
	resVars = new double [inputSize];
	knots = new double [numKnots];

	// Reset hash point of array
	zend_hash_internal_pointer_reset(Z_ARRVAL_P(data_x));	
	
	// Visit data of input and Dump to C++ array
	for (size_t i = 0; i < inputSize; i++)
	{
		// Dump data_x to co-variable
		zend_hash_get_current_data(Z_ARRVAL_P(data_x), (void**) &data_temp);	
		coVars[i] = (*data_temp)->value.dval;	
		
		// Dump data_y to response variable
		zend_hash_get_current_data(Z_ARRVAL_P(data_y), (void**) &data_temp);
		resVars[i] = (*data_temp)->value.dval;
		
		// Move forward next
		zend_hash_move_forward(Z_ARRVAL_P(data_y));
		zend_hash_move_forward(Z_ARRVAL_P(data_x));
	}

	// Reset hash point of array
	zend_hash_internal_pointer_reset(Z_ARRVAL_P(data_knots));
	for (size_t i = 0; i < numKnots; i++)
	{
		zend_hash_get_current_data(Z_ARRVAL_P(data_knots), (void**) &data_temp);	
		knots[i] = (*data_temp)->value.dval;
		zend_hash_move_forward(Z_ARRVAL_P(data_knots));	
		//RETURN_DOUBLE(knots[i]);
	}
		
	// Initialize result array
	MAKE_STD_ZVAL(result_array);	
	array_init(result_array);
	
	// Set knots for fitHandler
    fitHandler.setKnots(knots, numKnots);
	
	/*
	 * Here We supply three fitting methods thats are:
	 *  1) B-splines
	 *  2) Natural logarithm
	 *  3) Interval Greedy (fold line)
	 */
	
	/*
	 * Fitting parameters of B-spline functions
	 */
	paramVect = fitHandler.linearLeastSquares(numKnots, &CurveFitting::bsplineFit, 
			                                  coVars, resVars, inputSize);	
	// Dump paramVect to result_array
	for (int i = 0; i < numKnots; i++)
	{
		add_next_index_double(result_array, paramVect[i]);
	}
	delete[] paramVect;
	
	/*
	 * Fitting parameters of Natural logarithm
	 */
	paramVect = fitHandler.linearLeastSquares(NUM_LOG, &CurveFitting::natLogFit, coVars + 1, resVars + 1, inputSize - 1);	
	// Dump paramVect to result_array
	for (int i = 0; i < NUM_LOG; i++)
	{
		add_next_index_double(result_array, paramVect[i]);
	}
	delete[] paramVect;
	
	/*
	 * Interval Greedy
	 */
	paramVect = fitHandler.intervalGreedy(coVars, resVars, inputSize);	
	// Dump paramVect to result_array
	for (int i = 0; i < NUM_INTERVAL; i++)
	{
		add_next_index_double(result_array, paramVect[2 * i]);
		add_next_index_double(result_array, paramVect[2 * i + 1]);
	}
	delete[] paramVect;

	/*
	 * Final components of result_array:
	 *  1) [0-9] Parameters of B-splines
	 *  2) [10-11] Parameters of natural logarithm
	 *  3) [12-33] Coordinates of interval greedy points
	 */
	RETURN_ZVAL(result_array, true, true);
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
