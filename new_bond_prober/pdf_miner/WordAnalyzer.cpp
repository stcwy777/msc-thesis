/*************************************************
*
*   FileName:       WordAnalyzer.cpp
*   Author:         WangYun	
*   Date:           Jan 6 2010 
*   Description:    APIs implementation of Class 
*                   WordAnalyzer
*
**************************************************/


#include "WordAnalyzer.h"

WordAnalyzer::WordAnalyzer()
{
    // Call ICTCLAS initialize function
    /*
    if (!ICTCLAS_Init("/home/datagroup/wangyun/bond_prober/pdf_miner/"))
    {
        string errStatement = "Error initialize ICTCLAS";
        ERROR_ALERT(errStatement.c_str());
    }
    else
    {
	int nItem = ICTCLAS_ImportUserDictFile("/home/datagroup/wangyun/bond_prober/pdf_miner/BondInfo.dict");
	printf("%d Items imported from the User-defined dictionary\n",nItem);    
    }
    */
}

WordAnalyzer::~WordAnalyzer()
{
    //ICTCLAS_Exit();
}

/*
 * Function:     handleEngNumber
 * Description:  Remove comma in a English number 
 * Input:        textBuffer: text buffer to handle 
 *               textLength: length of text
 * Output:       None       
 * Return:       Handled string
 */
string WordAnalyzer::handleEngNumber(const char* textBuffer, size_t textLength)
{
    pcre* regCommaFilter;       // Regex for puncutation filter in English number
    string rsltString;          // Result stored in a new string
    int commaMatch[MAX_MATCH];  // Match result for comma
    int commaOffset = 0;        // Match offset of comma
    int numMatched;             // # of matched result
    int rmvLength;              // Length of remove characters

    setRegex(&regCommaFilter, "\\d(,\\s?)\\d");

    // Initialize const char to string
    rsltString = string(textBuffer, textLength);
     
    while ((numMatched = pcre_exec(regCommaFilter, NULL, rsltString.c_str(), rsltString.size(), 
                                   commaOffset, NO_OPTION, commaMatch, MAX_MATCH)) > 0)
    {
        //cout << string(rsltString.c_str() + commaMatch[0], commaMatch[1] - commaMatch[0]) << endl;
        rmvLength = commaMatch[3] - commaMatch[2];
        rsltString.erase(commaMatch[2], rmvLength);
        commaOffset = commaMatch[1];
    }

    pcre_free(regCommaFilter);

    return rsltString;
}

string WordAnalyzer::segmentParagraph(string paraBuffer)
{
    string segResult;           // Segment result
    string gbBuffer;            // Temporary buffer to store gb character

    // Transform buffer encoding to GB
    gbBuffer = m_CodeTransformer.utf2gb(paraBuffer); 

    // Run ICTCLAS process 
    ICTCLAS_ParagraphProcess(gbBuffer.c_str(), gbBuffer.size(), (char*)segResult.c_str(), CODE_TYPE_UNKNOWN, 0);
    // DEBUG
    fstream f;
    f.open("gb", ios::out | ios::trunc);
    f << segResult;
    f.close();
    // Transform buffer encoding back to UTF-8
    segResult = m_CodeTransformer.gb2utf(segResult);

    return segResult;
}
