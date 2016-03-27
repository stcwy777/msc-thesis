/*************************************************
*
*   FileName:       InfoMiner.h
*   Author:         WangYun	
*   Date:           Jan 12 2011
*   Description:    Definition of Class InfoMiner
*
**************************************************/


#ifndef INFOMINER_H
#define INFOMINER_H

#include "common.h"
/*
 * Comment Later
 */
typedef list <string> Pattern;
typedef list <string>::iterator PatternIter;
typedef list <string> Item;
typedef list <string>::iterator ItemIter;

typedef struct rule
{
    Pattern pattern;
    size_t  tarNum;
    string  defValue;
}Rule;


/*
 *  Class: InfoMiner          
 *  Description: Extract information in text resources   
 *
 */
class InfoMiner
{
private:
    /*
     * Attributes
     */

public:
    /*
     * Methods
     */
    int protype(const char* readIn, size_t len, size_t itemNum, Rule* rules, string date);
    int segmantic(const char* readIn, size_t len);
    /*
     * Construction methods
     */
    InfoMiner();
    virtual ~InfoMiner();
};

#endif

