/*************************************************
*
*   FileName:       WordAnalyzer.h
*   Author:         WangYun	
*   Date:           Jan 6 2010
*   Description:    Definition of class WordAnalyzer
*
**************************************************/


#ifndef WORDANALYZER_H
#define WORDANALYZER_H

#include "common.h"
#include "ICTCLAS50.h"
#include "codetransformer.h"

/*
 *  Class: WordAnalyzer
 *  Description: Analyze words 
 *
 */
class WordAnalyzer
{
private:
    /*
     * Attributes
     */
    string m_OriTextString;             // Original text string 
    CCodeTransformer m_CodeTransformer; // Code transformer
public:
    /*
     * Methods
     */
    string handleEngNumber(const char* textBuffer, size_t textLength);
    string segmentParagraph(string paraBuffer);

    /*
     * Construction methods
     */
    WordAnalyzer();
    virtual ~WordAnalyzer();
};

#endif

