/*************************************************
*
*   FileName:       InfoMiner.cpp
*   Author:         WangYun	
*   Date:           Jan 12 2011
*   Description:    APIs implementation of Class 
*                   Info Miner
*
**************************************************/


#include "InfoMiner.h"

InfoMiner::InfoMiner()
{

}

InfoMiner::~InfoMiner()
{

}

int InfoMiner::protype(const char* readIn, size_t len, size_t itemNum, Rule* rules, string date)
{
    pcre* regTarget = NULL;     // Regular object 
    PatternIter patIter;         
    ItemIter    itmIter;
    string tmpValue;
    Item items;                 // Store the list of items
    Item tips;                  // Store the list of sentences where the items found
    int numMatched = 0;
    int match[MAX_MATCH];
    int offset = 0;
    const char* subStr;
    int leftBorder;
    int leftTmp;
    int rightBorder;
    int rightTmp;
    
    // Insert discovery date
    items.push_back(date);    
    
    // Extract information based on each rule
    for (int i = 0; i < itemNum; i++)
    {
        for (patIter = rules[i].pattern.begin(); patIter != rules[i].pattern.end(); patIter++)
        {
            // Complie regular rule 
            setRegex(&regTarget, (*patIter).c_str());
            
            // Also used as flag
            tmpValue = ""; 

            // Match rule
            if ((numMatched = pcre_exec(regTarget, NULL, readIn, len,
                                        NO_OFFSET, NO_OPTION, match, MAX_MATCH)) > 0)
            {
                // Capture each sub target linked with symbol "_"
                for (int j = 1; j <= rules[i].tarNum; j++)
                {
                    pcre_get_substring(readIn, match, numMatched, j, &subStr);
                    
                    if (tmpValue != "")
                        tmpValue += "_";
                    tmpValue += string(subStr);
                    pcre_free_substring(subStr);
                }

                // Push item into item queue
                // cout << tmpValue << endl;
                items.push_back(tmpValue + " ");
                pcre_free(regTarget);

                // Find the sentence
                leftBorder = findLabel(readIn, "。", match[0], len, BACKWARD);
                if (leftBorder > -1)
                {
                    leftBorder += strlen("。"); 
                }
                
                leftTmp = findLabel(readIn, "，", match[0], len, BACKWARD);

                if (leftBorder < leftTmp)
                {
                    leftBorder = leftTmp; 
                    leftBorder += strlen("，");
                }
                
                leftTmp = findLabel(readIn, " ", match[0], len, BACKWARD);

                if (leftBorder < leftTmp)
                {
                    leftBorder = leftTmp;
                    leftBorder++;
                }
                
                 
                rightBorder = findLabel(readIn, "。", match[1], len, FORWARD);
                
                rightTmp = findLabel(readIn, "，", match[1], len, FORWARD);

                if (rightBorder > rightTmp && rightTmp > 0)
                {
                    rightBorder = rightTmp; 
                }
                
                rightTmp = findLabel(readIn, " ", match[1], len, FORWARD);
                if (rightBorder > rightTmp && rightTmp > 0)
                {
                    rightBorder = rightTmp;
                }
                
                // Validate border
                if (leftBorder == -1)
                {
                    leftBorder = 0;
                }
                if (rightBorder == -1)
                {
                    rightBorder = len - 1;
                }

                // Generate tips sentence
                pcre_get_substring(readIn, match, numMatched, 0, &subStr);
                tmpValue = string(readIn + leftBorder, match[0] - leftBorder) + "<font color=''red''>" 
                           + string(subStr) + "</font>" + string(readIn + match[1], rightBorder - match[1]);
                // cout <<leftBorder << ":" << tmpValue << endl;
                tips.push_back(tmpValue);
                pcre_free_substring(subStr);

                break;
            }
            else
            {
                pcre_free(regTarget);
            }
        }
        // Use default value
        if (tmpValue == "")
        {
            items.push_back(rules[i].defValue);
            tips.push_back("Null");
        }
    }
    // If the extract find the core data
    itmIter = items.begin();
    if ((*(++itmIter)) == "Null" && (*(++itmIter)) == "Null")
    {
        return -1;
    }
    // Insert item into database
    MySQL dbHandler;
    
    // Link Database
    dbHandler.connect(HOST, USER, PWD, DB);
    
    // Generate sql insert query for new product
    string sqlQuery = INSERT_QUERY;
    sqlQuery += "(";
    for (itmIter = items.begin(); itmIter != items.end(); itmIter++)
    {
        sqlQuery += "'" + *itmIter + "', ";
    }
    sqlQuery += "NULL, NULL)";
    
    dbHandler.sendQuery(sqlQuery.c_str());
    
    // Generate sql insert query for tips
    itmIter = items.begin();
    itmIter++;
    tips.push_front(*(++itmIter));

    sqlQuery = INSERT_QUERY_TIPS;
    sqlQuery += "(";
    for (itmIter = tips.begin(); itmIter != tips.end(); itmIter++)
    {
        sqlQuery += "'" + *itmIter +"', ";
    }
    sqlQuery += "NULL, NULL)";
    dbHandler.sendQuery(sqlQuery.c_str());
    dbHandler.disconnect();
    items.clear();
    tips.clear();
    /*
    fstream  demo("demo", ios_base::out | ios_base::app);
    for (itmIter = items.begin(); itmIter != items.end(); itmIter++)
    {
        demo << *itmIter << "\t";
    }
    demo << endl;
    */
    //pcre_free(regTarget);
    return 0;
}

int InfoMiner::segmantic(const char* readIn, size_t len)
{
    pcre* regClue = NULL;
    pcre* regTarget = NULL;
    string patClue;
    string tarValue;
    //string extValue;
    string pattern[2];
    string confWords[2];
    size_t numPattern;
    size_t numConfWords;
    size_t leftBorder;
    size_t leftComma;
    size_t rightBorder;
    size_t rightComma;
    int numMatched = 0;
    int match[MAX_MATCH];
    int tarMatch[MAX_MATCH];
    int offset = 0;
    const char* subStr;
    
    patClue = "托管(基本)?情况";
    //patTarget = "([\\d\\.]+)(亿|万)";
    
    /* 剩余年限 */
    pattern[0] = "(全部)托管";
    pattern[1] = "([\\d\\.]+)\\s*(亿|万)";
    
    /* 置信词组 */
    confWords[0] = "托管";
    confWords[1] = "证券登记";
    numConfWords = 2;

    setRegex(&regClue, patClue.c_str());
   
    // Locate target segment by key words
    if ((numMatched = pcre_exec(regClue, NULL, readIn, len,
                                NO_OFFSET, NO_OPTION, match, MAX_MATCH)) > 0)
    {
        // Set offset to position of clue
        numPattern = 2;
        
        for (int i = 0; i < numPattern; i++)
        {
            setRegex(&regTarget, pattern[i].c_str());
            offset = match[1];

            tarValue = "empty";

            while ((numMatched = pcre_exec(regTarget, NULL, readIn, len,
                                           offset, NO_OPTION, tarMatch, MAX_MATCH)) > 0)
            {
                //，
                leftBorder = findLabel(readIn, "。", tarMatch[0], len, BACKWARD);
                
                //leftComma = findLabel(readIn, "。", tarMatch[0], len, BACKWARD);
                //if (leftBorder < leftComma)
                {
                    //leftBorder = leftComma;
                }
                rightBorder = findLabel(readIn, "。", tarMatch[0], len, FORWARD);
                rightComma = findLabel(readIn, "，", tarMatch[0], len, FORWARD);
                if (rightBorder > rightComma)
                {
                    rightBorder = rightComma;
                }
                //cout << leftBorder << " " << rightBorder << i << endl; 
                if (i == 0 || (findLabel(readIn, confWords[0].c_str(), leftBorder, rightBorder, FORWARD) != -1
                    && findLabel(readIn, confWords[1].c_str(), leftBorder, rightBorder, FORWARD) != -1))
                {
                    pcre_get_substring(readIn, tarMatch, numMatched, 0, &subStr);
                    tarValue = string(subStr);
                    pcre_free_substring(subStr);
                    cout << tarValue << endl;
                    pcre_free(regTarget);
                    break;
                }
                else
                {
                    offset = tarMatch[1];
                }
            }
            if (tarValue != "empty")
            {
                break;
            }
            else
            {
                pcre_free(regTarget);
            }
        }
    }
    else
    {
        cout << "Missing Clue\n";
    }
    
    pcre_free(regClue); 
    return 0;
}

