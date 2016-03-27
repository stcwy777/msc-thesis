/*************************************************
*
*   FileName:       mathfunc.cpp
*   Author:         WangYun	
*   Date:           11/10/2009
*   Description:    implementation of math functions
*
**************************************************/


#include "mathfunc.h"

/*
 * Function:     d_swap
 * Description:  Swap two double value
 * Input:        @
 *               @y: co-variables 
 * Output:       None
 * Return:       None
 */
void d_swap(double* orig, double* goal)
{
    double temp;    // Temp for swap
    
    // Swap
    temp = *orig;
    *orig = *goal;
    *goal = temp;
}

/*
 * Function:     gauss_convolution
 * Description:  Compute convolution of input with a gaussian function
 * Input:        @imgIn: image
 *               @x: regression function
 *               @y: co-variables 
 *               @size: response variables
 *               @numVar: # of variables
 * Output:       None
 * Return:       @smth: fitted parameter vector
 */
/*
float gauss_convolution(UCHAR *imgIn, int x, int y, int size,int direct)
{
	float conv;
	int i,j;			// loop variable
	conv = 0;
	switch (direct)
	{
		// gauss smooth
		case BOTH:
			// convolution with twodimention gaussian function
			for (i=0;i<size;i++)
			{
				for (j=0;j<size;j++)
				{
					conv += imgIn[i*size+j] * GAUSSSMOOTH(x-j,y-i,GAUSSDEV);
				}
			}
			break;
		// gauss x deviation
		case XDIR:
			// convolution with twodimention gaussian x-deviation function
			for (i=0;i<size;i++)
			{
				for (j=0;j<size;j++)
				{
					conv += imgIn[i*size+j] * GAUSSXDEVIA(x-j,y-i,GAUSSDEV);
				}
			}
			break;
		// gauss y deviation
		case YDIR:
			// convolution with twodimention gaussian y-deviation function
			for (i=0;i<size;i++)
			{
				for (j=0;j<size;j++)
				{
					conv += imgIn[i*size+j] * GAUSSYDEVIA(x-j,i-y,GAUSSDEV);
				}
			}
			break;
		default:
			break;
	}
	return conv;
}
*/

/*
 * Function:     matrix_transpose
 * Description:  Transpose a matrix 
 * Input:        @mat_input: input matrix 
 *               @num_row: # of row 
 *               @num_col: # of column
 * Output:       @mat_output: output matrix
 * Return:        0: Successfully execute
 *               -1: Error occured
 */
int matrix_transpose(Matrix mat_input, Matrix mat_output, int num_row, int num_col)
{ 
    // Transpose mat[i,j] to mat[j,i]
    for (size_t i = 0; i < num_row; i++)
    {
        for (size_t j = 0; j < num_col; j++)
        {
            mat_output[j * num_row + i] = mat_input[i * num_col + j];
        }
    }
    return 0;
}


/*
 * Function:     matrix_inverse
 * Description:  Generate inverse matrix of input 
 * Input:        @mat_input: input matrix 
 *               @order: order of matrix
 * Output:       @mat_output: output matrix
 * Return:        0: Successfully execute
 *               -1: Error occured
 */
int matrix_inverse(Matrix mat_input, Matrix mat_output, size_t order)
{
    double  max_main_meta;      // Max main-meta
    double  temp;               // Temp value in seanum_colhing max main-meta
    size_t* row_records;        // Row records of replace position
    size_t* col_records;        // Column records of replace position
    size_t  replace_index;      // Index for replace

    // Initialize inverse matrix
    for (size_t i = 0; i < order * order; i++)
    {
        mat_output[i] = mat_input[i];
    }

    row_records = new size_t [order];
    col_records = new size_t [order]; 
    
    // Generate inverse matrix
    for (size_t k = 0; k < order; k++)
    {
        // Step1: find max main-meta all-select
        max_main_meta = 0.0;
        for (size_t i = k; i < order; i++)
        {
            for (size_t j = k; j < order; j++)
            {
                temp = fabs(mat_output[i * order + j]);
                // Mark replace position in records set
                if (max_main_meta < temp)
                {
                    max_main_meta = temp;
                    row_records[k] = i;
                    col_records[k] = j;
                }
            }
        }
        
        // When max main-meta less than threshold overflow error may occur
        if (max_main_meta < THRESHOLD)
        {
            fprintf(stderr, "Overflow error: max main-meta less than threshold\n"); 
            return -1;
        }

        // Replace matrix if needed
        if (row_records[k] != k)
        {
            matrix_replace(mat_output, order, k, row_records[k], ROW);
        }

        if (col_records[k] != k)
        {
            matrix_replace(mat_output, order, k, col_records[k], COL);			
        }

        // Step2: m(k,k) = 1 / m(k,k), m(k,k) is max main-meta
        mat_output[k * order + k] = 1 / mat_output[k * order + k];

        // Step3: m(k,j) = m(k,j) * m(k,k) other elements of current row should divide max main-meta
        for (size_t j = 0; j < order; j++)
        {
            if (j != k)
            {
                mat_output[k * order + j] = mat_output[k * order + j] * mat_output[k * order + k];
            }
        }

        // Step4: m(i,j) = m(i,j) - m(k,j) * m(i,k) (i,j != k)
        for (size_t i = 0; i < order; i++)
        {
            if (i != k)
            {
                for (size_t j = 0; j < order; j++)
                {
                    if (j != k)
                    {
                        mat_output[i * order + j] -= mat_output[k * order + j] * mat_output[i * order + k];
                    }
                }
            }
        }

        // Step5: m(i,k) = - m(i,k) / m(k,k) | standing inverse, fix column k
        for (size_t i = 0; i < order; i++)
        {
            if (i != k)
            {
                mat_output[i * order + k] = (-1) * mat_output[i * order + k] * mat_output[k * order + k];
            }
        }
    }
    

    // Step6: restore replacement
    for (size_t k = 0; k < order; k++)
    {
        replace_index = order - k -1;
        if (row_records[replace_index] != replace_index)
        {
            matrix_replace(mat_output, order, replace_index, row_records[replace_index], COL);
        }       
        if (col_records[replace_index] != replace_index)
        {
            matrix_replace(mat_output, order, replace_index, row_records[replace_index], ROW);
        }
    }

    // Clear allocated space
    delete[] row_records;
    delete[] col_records;
    return 0;
}


/*
 * Function:     linear_equation
 * Description:  Compute solution of linear equations 
 * Input:        @mat_coef: matrix of coefficient
 *               @mat_rslt: matrix of result
 *               @order: order of linear equations
 * Output:       None
 * Return:        0: Successfully execute
 *               -1: Error occured
 */
int linear_equation(Matrix mat_coef, Matrix mat_rslt, size_t order)
{
    double  max_main_meta;      // Max main-meta
    double  temp;               // Temp value in seanum_colhing max main-meta
    size_t* row_records;        // Row records of replace position
    size_t* col_records;        // Column records of replace position
    size_t  replace_index;      // Index for replace

    //row_records = (size_t*)malloc(order * sizeof(size_t));
    //col_records = (size_t*)malloc(order * sizeof(size_t));
    row_records = new size_t[order];
    col_records = new size_t[order];
    
    // Find solution by Gauss-Jordan method
    for (size_t k = 0; k < order; k++)
    {
        // Step1: find max main-meta all-select
        max_main_meta = 0.0;
        for (size_t i = k; i < order; i++)
        {
            for (size_t j = k; j < order; j++)
            {
                temp = fabs(mat_coef[i * order + j]);
                /* 
                 * Mark replace position in records set
                 * both pos_k of row & col records will be initialize to k
                 */
                if (max_main_meta < temp)
                {
                    max_main_meta = temp;
                    row_records[k] = i;
                    col_records[k] = j;
                }
           }
        }

        // When max main-meta less than threshold overflow error may occur
        /*
        if (max_main_meta < THRESHOLD)
        {
            fprintf(stderr, "Overflow error: max main-meta less than threshold\n"); 
            return -1;
        }
        */
        // Replace matrix if needed
        if (row_records[k] != k)
        {
            matrix_replace(mat_coef, order, k, row_records[k], ROW);
            d_swap(&mat_rslt[k], &mat_rslt[row_records[k]]);
        } 

        if (col_records[k] !=k)
        {
           matrix_replace(mat_coef, order, k, col_records[k],COL);
        }

        // Step2: coef(k,k) = 1 / coef(k,k)
        mat_coef[k * order + k] = 1 / mat_coef[k * order + k];

        for (size_t j = 0; j < order; j++)
        {
            if (j != k)
           {
                mat_coef[k * order + j] = mat_coef[k * order + j] * mat_coef[k * order + k];
           }
        }
        mat_rslt[k] = mat_rslt[k] * mat_coef[k * order + k];


        for (size_t i = 0; i < order; i++)
        {
            if (i != k)
            {
                for (size_t j = 0; j < order; j++)
                {
                    if (j != k)
                    {
                        mat_coef[i * order + j] = mat_coef[i * order + j] - mat_coef[k * order + j] * mat_coef[i * order + k];
                    }
                }
                mat_rslt[i] = mat_rslt[i] - mat_rslt[k] * mat_coef[i * order + k];
            }
        }
        
    }
 
    // Step6: restore replacement
    for (size_t k = 0; k < order; k++)
    {
        replace_index = order - k -1;
        if (col_records[replace_index] != replace_index)
        {
           d_swap(&mat_rslt[replace_index], &mat_rslt[col_records[replace_index]]);
        }
    }
    // Clear allocated space
    //free(row_records);
    //free(col_records);
    delete[] row_records;
    delete[] col_records;
    return 0;
}


/*
 * Function:     matrix_replace
 * Description:  Replace two row or column of a matrix 
 * Input:        @mat_input: input matrix
 *               @order: order of matrix
 *               @orig: original position
 *               @goal: goal position
 *               @mode: replace row or column
 * Output:       None
 * Return:        0: Successfully execute
 *               -1: Error occured
 */
int matrix_replace(Matrix mat_input, size_t order, size_t orig, size_t goal, size_t mode)
{
    // Check param;
    if (order <= 0 || orig < 0 || goal < 0 || orig == goal)
    {
        fprintf(stderr, "illegal param in matrix replacement\n");
        return -1;
    }
    for (size_t i = 0; i < order; i++)
    {
        // Replace two rows
        if (mode == ROW)
        {
            d_swap(&mat_input[orig * order + i], &mat_input[goal * order + i]);
        }
        // Replace two columns
        else if (mode == COL)
        {
            d_swap(&mat_input[i * order + orig], &mat_input[i * order + goal]);
        }
        else
        {
            fprintf(stderr, "illegal mode in matrix replacement\n");
            return -1;
        }
    }
    return 0;
}

/*
 * Function:     matrix_multiply
 * Description:  Multiply two matrix 
 * Input:        @mat_left: left input matrix
 *               @mat_rght: right input matrix
 *               @num_row: # of rows of left matrix
 *               @num_col: # of columns of right matrix
 *               @num_comm: # of columns of left matrix or rows of right matrix
 * Output:       @mat_rslt: result matrix
 * Return:        0: Successfully execute
 *               -1: Error occured
 */
int matrix_multiply(Matrix mat_left, Matrix mat_rght, Matrix mat_rslt, size_t num_row, size_t num_col, size_t num_comm)
{
    // Check param;
    if (num_row <= 0 || num_col <= 0 || num_comm <= 0)
    {
        fprintf(stderr, "Illegal param in matrix multiply\n");
        return -1;
    }

    // Multiply two matrix
    for (size_t i = 0; i < num_row; i++)
    {
        for (size_t j = 0; j < num_col; j++)
        {
            mat_rslt[i * num_col + j] = 0;
            for (size_t k = 0; k < num_comm; k++)
            {
                mat_rslt[i * num_col + j] += mat_left[i * num_comm + k] * mat_rght[k * num_col + j];
            }
        }
    }
    return 0;
}

/*
 * Function:     matrix_num_multiply
 * Description:  Multiply a matrix with a number
 * Input:        @mat_input: left input matrix
 *               @num_row: # of rows of matrix
 *               @num_col: # of columns of matrix
 *               @num: the number
 * Output:       None
 * Return:        0: Successfully execute
 *               -1: Error occured
 */
int matrix_num_multiply(Matrix mat_input, size_t num_row, size_t num_col, double num)
{
    // Check param;
    if (num_row <= 0 || num_col <= 0)
    {
        fprintf(stderr, "Illegal param in matrix number multiply\n");
        return -1;
    }
    
    // Mutiply each element with num
    for (size_t i = 0; i < num_row; i++)
    {
        for (size_t j = 0; j < num_col; j++)
        {
            mat_input[i * num_col + j] *= num; 
        }
    }
    return 0;
}

/*
 * Function:     matrix_plus
 * Description:  Plus two matrix
 * Input:        @mat_left: left input matrix
 *               @mat_rght: right input matrix
 *               @num_row: # of rows of matrix
 *               @num_col: # of columns of matrix
 *               @neg: when neg = -1 plus come to minus
 * Output:       @mat_rslt: result matrix
 * Return:        0: Successfully execute
 *               -1: Illegal param
 *               -2: Zero result matrix
 */
int matrix_plus(Matrix mat_left, Matrix mat_rght, Matrix mat_rslt, size_t num_row, size_t num_col,int neg)
{
    size_t zero_check;      // Check num of zero element	

    // Check param;
    if (num_row <= 0 || num_col <= 0 )
    {
        fprintf(stderr, "Illegal param in matrix plus\n");
        return -1;
    }
	
    // Initialize 
    zero_check = 0;
    // Plus matrix point by point
    for (size_t i = 0; i < num_row; i++)
    {
        for (size_t j = 0; j < num_col; j++)
	   {
            mat_rslt[i * num_col + j] = mat_left[i * num_col + j] + neg * mat_rght[i * num_col + j];
            if (mat_rslt[i * num_col + j] < THRESHOLD)
            {
                zero_check++;
            }
        }
    }
    // 
    if (zero_check == num_row * num_col)
    {
        fprintf(stderr, "Warning: Result matrix is zero\n");
        return -2;
    }
    return 0;
}
