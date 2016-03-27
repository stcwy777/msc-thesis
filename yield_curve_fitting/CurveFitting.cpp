/*************************************************
*
*   FileName:       CurveFitting.cpp
*   Author:         WangYun	
*   Date:           Jul.16 2010
*   Description:    APIs implementation of Class 
*                   CurveFitting 
*
**************************************************/


#include "CurveFitting.h"

CurveFitting::CurveFitting()
{
    m_SplineKnots.values = NULL;
}

CurveFitting::~CurveFitting()
{
    if (m_SplineKnots.values != NULL)
    {
        delete[] m_SplineKnots.values;
    }
}

/*
 * Function:     linearLeastSquares
 * Description:  Fit parameter vectors of linear regression by least squares 
 * Input:        @numUnits: # of units of linear regression
 *               @regFunc: regression function
 *               @coVar: co-variables 
 *               @resVar: response variables
 *               @numVar: # of variables
 * Output:       None
 * Return:       @parVect: fitted parameter vector
 */
double* CurveFitting::linearLeastSquares(size_t numUnits, double (CurveFitting::* regFunc)(size_t, double), 
                                         double* coVar, double* resVar, size_t numVar) {
    double* parVect;            // Parameter vector of linear regression
    double* coefMatrix;         // Coefficient matrix
    double* rsltMatrix;         // Result matrix

    // Allocate space
    coefMatrix = new double [numUnits * numUnits];
    rsltMatrix = new double [numUnits];
    
    // Calculate matrice
    for (size_t i = 0; i < numUnits; i++)
    {
        // Calculate first row of coefficient matrix
        for (size_t j = 0; j < numUnits; j++)
        {
            coefMatrix[i * numUnits + j] = 0;
            // Calculate sum of 2-norm of cardinal i & j
            for (size_t k = 0; k < numVar; k++)
            {
                 coefMatrix[i * numUnits + j] += (this->*regFunc)(i, coVar[k]) * (this->*regFunc)(j, coVar[k]);
            }
        }
        // Calculate first row of result matrix
        rsltMatrix[i] = 0;
        for (size_t j = 0; j < numVar; j++)
        {
            rsltMatrix[i] += resVar[j] * (this->*regFunc)(i, coVar[j]);
        }
    }

    // Solve learn equation with extension matrix [coefMatrix:rsltMatrix]
    linear_equation(coefMatrix, rsltMatrix, numUnits);
    // Linear equation store result in rsltMatrix by standing-method 
    parVect = rsltMatrix;

    delete[] coefMatrix;
    return parVect;
}

void CurveFitting::interpolation(size_t numUnits, double (CurveFitting::* regFunc)(size_t, double), 
                                 double* parVect, double* coVar, double* resVar, size_t numVar)
{
    for (size_t i = 0; i < numVar; i++)
    {
        resVar = 0;
        // Calculate response variable by sum of regression units
        for(size_t j = 0; j < numUnits; j++)
        {
            resVar[i] += parVect[j] * (this->*regFunc)(j, coVar[i]);
        }
    }
}

/*
 * Function:     natLogFit
 * Description:  Natural logarithm function for curve fitting:
 * Input:        @seq: sequence of linear units
 *               @coVar: covariable 
 * Output:       None
 * Return:       @resVar: response variable
 */
double CurveFitting::natLogFit(size_t seq, double coVar)
{
    double resVar;      // Response value of covariable
    
    // Return 1 for units zero 
    if (seq == 0)
    {
        resVar = 1;
    }
    // Return natural logarithm of covariable 
    else
    {
        resVar = log(coVar);
    }

    return resVar;
}

/*
 * Function:     polyFit
 * Description:  Polynomial function for curve fitting:
 * Input:        @seq: sequnce of linear units
 *               @coVar: covariable 
 * Output:       None
 * Return:       @resVar: response variable
 */
double CurveFitting::polyFit(size_t seq, double coVar)
{
    double resVar;      // Response value of covariable
    
    // Return 1 for units zero 
    if (seq == 0)
    {
        resVar = 1;
    }
    // Return order-th power of covariable 
    else
    {
        resVar = pow(coVar, seq);
    }

    return resVar;
}

/*
 * Function:     bspline
 * Description:  B-spline function
 * Input:        @order: order of B-spline
 *               @knots: knots of current B-spline series functions
 *               @seq: sequence of function seq-th B-spline
 *               @coVar: covariable 
 * Output:       None
 * Return:       @resVar: response variable
 */
double CurveFitting::bspline(size_t order, Knots& knots, size_t seq, double coVar)
{
    double  resVar = 0;         // Response value of covariable
    double  tempDividend;       // Temp value of dividend
    double  tempDivisor;        // Temp value of divispr
    size_t  upperBond;          // Uppder bond of knots sequence

    // Expection upperbond sequence of knot
    upperBond = seq + order + 1;    
    // Fix to knots' amount if expection uppderbond exceed the amount
    if (upperBond > knots.amount)
    {
        upperBond = knots.amount; 
    }

    /*
     * Here we have (upperBond - seq) sub interval
     * If covariable less than knots[seq] or great than knots[upperBond]
     * B-spline will return 0.
     * Otherwise, return response value based on each sub domain contains covariable  
     */
    if (coVar > knots.values[seq] && coVar <= knots.values[upperBond])
    {
        for (size_t i = seq; i <= upperBond; i++)
        {
            // Stop when covariable less than left bond of current sub interval
            if (coVar < knots.values[i])
            {
                break;
            }
            else
            {
                // Initialize temp value according to B-spline equation 
                tempDividend = pow((coVar - knots.values[i]), order);
                tempDivisor = 1;
                // Visit each kont in [seq, upperBond]     
                for (size_t j = seq; j <= upperBond; j++)
                {
                    // Skip same knot
                    if ( j == i)
                    {
                        continue;
                    }
                    // Multiply distance from every different knot to current one
                    else
                    {
                        tempDivisor *= (knots.values[j] - knots.values[i]);
                    }
                }
            }
            // Acculate value in each sub interval
            resVar += tempDividend / tempDivisor;
        }
    }
   
    return resVar;
}

/*
 * Function:     bsplineFit
 * Description:  Use B-spline function for curve fitting
 * Input:        @seq: sequnce of linear units
 *               @coVar: covariable 
 * Output:       None
 * Return:       @resVar: response variable
 */
double CurveFitting::bsplineFit(size_t seq, double coVar)
{
    double resVar;      // Response value of covariable
    
    // Return 1 for units zero 
    if (seq == 0)
    {
        resVar = 1;
    }
    // Return response of covariable with specific B-splines 
    else
    {
        resVar = bspline(CUBIC, m_SplineKnots, seq, coVar);
    }

    return resVar;
}

/*
 * Function:     setKnots
 * Description:  Set spline knots by user input
 * Input:        @values: knots values
 *               @amount: # of knots
 * Output:       None
 * Return:       None
 */
void CurveFitting::setKnots(double* values, size_t amount)
{
    // Set amount of knots
    m_SplineKnots.amount = amount;
    
    // Clear last set If knots have been set
    if (m_SplineKnots.values != NULL)
    {
        delete[] m_SplineKnots.values;
        m_SplineKnots.values = NULL;
    }

    // Attention index of valuse begin with 1
    m_SplineKnots.values = new double [amount + 1];
    
    // Set values by user input
    for (size_t i = 1; i <= amount; i++)
    {
        m_SplineKnots.values[i] = values[i - 1];
    }
}

double* CurveFitting::intervalGreedy(double* coVar, double* resVar, size_t numVar)
{
    size_t  interIndex;                 // Interval index to which the coVar belongs
    size_t* interAmount;                // # of samples in each interval
    double* sumCoVar;                   // Array of sum of co-variables
    double* sumResVar;                  // Array of sum of response variables
    double* interOptimal;               // Optimal point in each interval

    // Entries set by ad hoc
    size_t  entries[26] = {0, 1, 2, 3, 4, 5, 6,
                           7, 7, 
                           8, 8, 8, 8, 8, 8,
                           9, 9, 9, 9, 9, 9,
                           9, 9, 9, 9, 9};

    sumCoVar = new double [10];
    sumResVar = new double [10];
    interAmount = new size_t [10];
    interOptimal = new double [20];
    
    memset(sumCoVar, NULL, sizeof(double) * 10);
    memset(sumResVar, NULL, sizeof(double) * 10);
    memset(interAmount, NULL, sizeof(size_t) * 10);
    
    for (size_t i = 0; i < numVar; i++)
    {
        interIndex = (size_t)coVar[i];
        
        sumCoVar[entries[interIndex]] += coVar[i];
        sumResVar[entries[interIndex]] += resVar[i];
        interAmount[entries[interIndex]]++;
    }

    for (size_t i = 0; i < 10; i++)
    {
        interOptimal[2 * i] = sumCoVar[i] / interAmount[i];
        interOptimal[2 * i + 1] = sumResVar[i] / interAmount[i];
    }
    
    delete[] sumCoVar;
    delete[] sumResVar;
    delete[] interAmount;

    return interOptimal;
}
