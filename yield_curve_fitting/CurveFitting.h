/*************************************************
*
*   FileName:       CurveFitting.h
*   Author:         WangYun	
*   Date:           Jul.16 2010
*   Description:    Definition of Class CurveFitting
*                   
**************************************************/


#ifndef CURVEFITTING_H
#define CURVEFITTING_H

#include "mathfunc.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <iostream>
#include <fstream>
#include <string>

// Custom Marco
#define CUBIC   3

/*
 *  Structure: Knots
 *  Description: Used in spline functions    
 *
 */
typedef struct knots
{
    double* values;         // Value of knots (array index begin with 1)
    size_t  amount;         // Amount of total knots 
}Knots;

/*
 *  Class: CurveFitting          
 *  Description: Supply API for curve fitting    
 *
 */
class CurveFitting
{
private:
    /*
     * Attributes
     */
    Knots       m_SplineKnots;          // Knots used for spline functions
    //size_t*     m_IntervalEntry;        // Entry of intervals used for Greedy choose

public:
    /*
     * Methods
     */
    double* linearLeastSquares(size_t numUnits, double (CurveFitting::* regFunc)(size_t, double), 
                               double* coVar, double* resVar, size_t numVar);
    double natLogFit(size_t base, double coVar);
    double polyFit(size_t order, double coVar);
    double bspline(size_t order, Knots& knots, size_t seq, double coVar);
    double bsplineFit(size_t seq, double coVar);
    void setKnots(double* values,size_t amount);
    void interpolation(size_t numUnits, double (CurveFitting::* regFunc)(size_t, double), 
                       double* parVect, double* coVar, double* resVar, size_t numVar);
    double* intervalGreedy(double* coVar, double* resVar, size_t numVar);
    /*
     * Construction methods
     */
    CurveFitting();
    virtual ~CurveFitting();
};

#endif

