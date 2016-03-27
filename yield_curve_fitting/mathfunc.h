/*********************************************
*
*   FileName:       mathfunc.h
*   Author:         WangYun	
*   Date:	    11/10/2009
*   Description:    headfile fo math function
*				
*
**********************************************/


#ifndef MATHFUNC_H
#define MATHFUNC_H

#ifdef __cplusplus 
extern "C" { 
#endif 
    
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <math.h>

// Custom type
typedef double* Matrix;

// Macros
#define PI 3.1415926
#define ROW 1
#define COL 0
#define GAUSSDEV 0.9
#define THRESHOLD 0.00001

// Gaussion Functions
#define GAUSSSMOOTH(x,y,d) ( exp( (-1) * ( (x)*(x) + (y)*(y)) / (2 * d * d) ) )
#define GAUSSXDEVIA(x,y,d) ( exp( (-1) * ( (x)*(x) + (y)*(y)) / (2 * d * d) ) * (-1) * ((x) / (d * d) ) )
#define GAUSSYDEVIA(x,y,d) ( exp( (-1) * ( (x)*(x) + (y)*(y)) / (2 * d * d) ) * (-1) * ((y) / (d * d) ) )
#define BOTH 0
#define XDIR 1
#define YDIR 2


//float GaussConvolution(UCHAR *imgIn, int x, int y, int size,int direct);
void d_swap(double* orig, double* goal);
int matrix_transpose(Matrix mat_input, Matrix mat_output, int num_row, int num_col);
int matrix_inverse(Matrix mat_input, Matrix mat_output, size_t order);
int linear_equation(Matrix mat_coef, Matrix mat_rslt, size_t order);
int matrix_replace(Matrix mat_input, size_t order, size_t orig, size_t goal, size_t mode);
int matrix_multiply(Matrix mat_left, Matrix mat_rght, Matrix mat_rslt, size_t num_row, size_t num_col, size_t num_comm);
int matrix_num_multiply(Matrix mat_input, size_t num_row, size_t num_col, double num);
int mat_plus(Matrix mat_left, Matrix mat_rght, Matrix mat_rslt, size_t num_row, size_t num_col,int neg);
#ifdef __cplusplus 
} 
#endif 

#endif
