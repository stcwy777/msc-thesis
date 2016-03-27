/*************************************************
*
*    FileName:      common.h
*      Author:      WangYun	
*        Date:      Mar.18 2010
* Description:      Common headfile and macros 
*                   for PDF2TextConv
*
**************************************************/

#ifndef COMMON_H
#define COMMON_H

// Linux C head file
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <sys/types.h>
#include <regex.h>
#include <string.h>

// C++ head file
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <set>
#include <list>
#include <sstream>
using namespace std;

// External head file
#include <pcre.h>
// MySQL
#include "MySQL.h"
//#define HOST "219.223.242.210"
#define HOST "219.223.242.249"
//#define HOST "localhost"
#define USER "root"
//#define PWD "hitinsun"
#define PWD "hitsz"
//#define PWD "wang"
//#define DB "haitianyuan"
#define DB "bond"
//const string INSERT_QUERY = "INSERT INTO `haitianyuan`.`zzz_bond` (`date`, `name`, `code`, `interest`, `total`, `credit`, `guarantee`, `due_time`, `beg_time`, `period`, `addr`, `main_credit`, `trusteeship`, `remark`) VALUES ";
const string INSERT_QUERY = "INSERT INTO `bond`.`zzz_bond` (`date`, `name`, `code`, `interest`, `total`, `credit`, `guarantee`, `due_time`, `beg_time`, `period`, `addr`, `main_credit`, `trusteeship`, `remark`) VALUES ";

//const string INSERT_QUERY_TIPS = "INSERT INTO `haitianyuan`.`zzz_bond_tips` (`label`, `name`, `code`, `interest`, `total`, `credit`, `guarantee`, `due_time`, `beg_time`, `period`, `addr`, `main_credit`, `trusteeship`, `remark`) VALUES ";
const string INSERT_QUERY_TIPS = "INSERT INTO `bond`.`zzz_bond_tips` (`label`, `name`, `code`, `interest`, `total`, `credit`, `guarantee`, `due_time`, `beg_time`, `period`, `addr`, `main_credit`, `trusteeship`, `remark`) VALUES ";

#define CHECKFILE(state, path) \
                if (!state) \
                {\
                    fprintf(stderr, "At file %s, line %d: \nFile open error: %s\n", __FILE__, __LINE__, path); \
                    exit(-1);\
                }\

#define ERROR_ALERT(statement) \
                {\
                    fprintf(stderr, "At file %s, line %d: \n%s\n", __FILE__, __LINE__, statement); \
                    exit(-1);\
                }\

// Define const value
const int CR = 0x0D;
const int LF = 0x0A;

// Const value for PCRE
const size_t MAX_MATCH = 30;                // Max # of matched sub strings 
const int NO_OPTION = 0;                    // No options 
const int NO_OFFSET = 0;                    // No offset

// Const value for label searching
const int FORWARD = 1;
const int BACKWARD = -1;

// Check whether character is a number or not
inline int isNumber(char charValue)
{
    if (charValue <= 0x39 && charValue >= 0x30)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
// Transform hex string to integer
inline int hex2int(const char* hexStr)
{
    int intVar;
    sscanf(hexStr, "%x", &intVar);
    /*
    if (intVar == 1082 || intVar == 20070 || intVar == 36710)
    {
        fstream ftest;
        ftest.open("code.txt", ios::out | ios::app);
        ftest << hexStr << endl;
        ftest.close();
    }
    */
    return intVar;
}
// Transform integer to string
inline string int2str(int num)
{
    stringstream ostream;
    ostream << num;
    return ostream.str();
}
// File size in byte
inline u_int fileSize(ifstream& file)
{
    u_int size;

    file.seekg(0, ios::end);
    size = file.tellg();
    file.seekg(0, ios::beg);

    return size;
}
// String split
inline vector<string>& split(string& str, char delim, vector<string>& elems)
{
    stringstream ss(str);
    string item;
    elems.clear();

    while(getline(ss, item, delim)) 
    {
        elems.push_back(item);
    }
    return elems;
}

// Transfomr UCS2 to UTF-8
inline u_int ucs2utf(char* utfCode, u_short ucsCode)
{
    u_int   size;           // Tranformed code size
    /*   
    (1)if UCS < 0x007F(00000000 - 01111111), tranlated to ASCII； 
    (2)if UCS < 0x07FF, tranlate to 2 bytes, 11 bits from right side insert into 110xxxxx 10yyyyyy
       etc. 00000aaa bbbbbbbb ==> 110aaabb 10bbbbbb 
    (3)else translate to 3 bytes, 16 bits from right sied insert blank space
       etc. aaaaaaaa bbbbbbbb ==> 1110aaaa 10aaaabb 10bbbbbb 
    */
    if (ucsCode <= 0x007F)
    {
        utfCode[0] = ucsCode;
        size = 1;
    }
    else if (ucsCode <= 0x07FF)
    {
        utfCode[0] = 0xC0 + (ucsCode >> 6);
        utfCode[1] = 0x80 + (ucsCode & 0x003F); 
        size = 2;
    }
    else
    {
        utfCode[0] = 0xE0 + (ucsCode >> 12);
        utfCode[1] = 0x80 + ((ucsCode & 0x0FC0) >> 6);
        utfCode[2] = 0x80 + (ucsCode & 0x003F);
        size = 3;
    }   
    return size;
}
/*
 * Function:     setRegex
 * Description:  Compile regex for specific pattern
 * Input:        @regex: pcre regex structure
 *               @patter: pattern string
 * Output:       None       
 * Return:       ot: Error occured
 *               0:  Successfully executed
 */
inline int setRegex(pcre** regex, const char* pattern)
{
    int errPos;                     // Where error occured
    const char* errMsg = NULL;      // Error message   
    
    // Compile regex for pattern string using PCRE API
    *regex = pcre_compile(pattern, PCRE_MULTILINE | PCRE_DOTALL, &errMsg, &errPos, NULL);
    //*regex = pcre_compile(pattern, PCRE_CASELESS | PCRE_MULTILINE, &errMsg, &errPos, NULL);
    
    // Error occured
    if (*regex == NULL)
    {
        printf("Can't compile regex %s (ErrorMsg:%s Pos:%d)\n", pattern, errMsg, errPos);
        pcre_free_substring(errMsg);
        return -1;
    }
    
    return 0;
}

/*
 * Function:     findObject
 * Description:  Find syntax objects in PDF temporary buffer
 * Input:        @buffer: source string 
 *               @obj: name of object
 *               @len: max length of buffer
 *               @offset: offset of start pos
 *               @direction: search direction FORWARD / BACKWARD
 * Output:       None 
 * Return:       -1: Not found
 *               >0: Start position of object
 */
inline int findLabel(const char* buffer, const char* obj, size_t offset, size_t len, int direction)
{
    int objLength;        // String length of object phrase
    int matchedLength;    // Matched length during search
    size_t pos;           // Position of object in buffer

    // Initialize variables before searching	
    objLength = strlen(obj);				
     
    if (direction == FORWARD)
    {
        matchedLength = 0;
        // Start searching by bytes
        for (size_t i = offset; i < len; i++)
        {
            // Matched one char
            if (buffer[i] == obj[matchedLength])
            {
                // Increase matchedLength
                matchedLength++;
                if (matchedLength == objLength)
                {
                    // Achive the length and find the position
                    pos = i - objLength + 1;
                    return pos;
                }
            }
            // Mismatch reset matchedLength
            else
            {
                matchedLength = 0;
            }
        }
        return -1;
    }
    else if (direction == BACKWARD)
    {
        matchedLength = objLength - 1;
        // Start searching by bytes
        for (int i = offset; i >= 0; i--)
        {
            // Matched one char
            if (buffer[i] == obj[matchedLength])
            {
                // Increase matchedLength
                matchedLength--;
                if (matchedLength < 0)
                {
                    // Achive the length and find the position
                    pos = i;
                    return pos;
                }
            }
            // Mismatch reset matchedLength
            else
            {
                matchedLength = objLength - 1;
            }
        }    
        // End rotation unable to find
        return -1;
    }
}
#endif 
