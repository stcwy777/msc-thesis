/*************************************************
*
*   FileName:       MySQL.h
*   Author:         WangYun	
*   Date:           Jul.12 2010
*   Description:    Definition of Class MySQL
*
**************************************************/


#ifndef MYSQL_H
#define MYSQL_H

#include "mysql/mysql.h"
#include "common.h"

// Macro define
#define UNIX_SOCKET "/tmp/mysql.sock"

/*
 *  Class: MySQL          
 *  Description: Access MySQL Database    
 *
 */
class MySQL 
{
private:
    /*
     * Attributes
     */
    MYSQL   m_MySQL;            // MySQL handle for follow operations
    MYSQL_RES*  m_Result;       // MySQL result structure

public:
    /*
     * Methods
     */
    int connect(const char* host, const char* user, const char* pwd, const char* database);
    int sendQuery(const char* sqlQuery);
    void clearBuff(char** &buff, size_t numFields);
    void disconnect();
    size_t fetchRowData(char** &recvBuff);
    
    /*
     * Construction methods
     */
    MySQL();
    virtual ~MySQL();
};

#endif

