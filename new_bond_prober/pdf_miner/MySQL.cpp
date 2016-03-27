/*************************************************
*
*   FileName:       MySQL.cpp
*   Author:         WangYun	
*   Date:           Jul.12 2010
*   Description:    APIs implementation of Class 
*                   MySQL
*
**************************************************/


#include "MySQL.h"

MySQL::MySQL()
{
    // Initialize MySQL C API & handle
    mysql_library_init(0, NULL, NULL);
    mysql_init(&m_MySQL);
}

MySQL::~MySQL()
{
    mysql_library_end();
}

/*
 * Function:     readPdf
 * Description:  Read data from PDF file
 * Input:        @path: File path of PDF
 * Output:       None
 * Return:       -1: Error occured
 *                0: Successfully executed
 */
int MySQL::connect(const char* host, const char* user, const char* pwd, const char* database)
{
    // Link MySQL server use specific socket
    mysql_options(&m_MySQL, MYSQL_SET_CHARSET_NAME, "utf8");
    if(!mysql_real_connect(&m_MySQL, host, user, pwd, database, 0, NULL, 0))
    {
        fprintf(stderr, "Failed to connect to database: Error: %s\n", mysql_error(&m_MySQL));
        return -1;
    }
    else
    {
        printf("Successfully connect to database:%s of %s\n", database, host);
        return 0;
    }
}

/*
 * Function:     sendQuery
 * Description:  Send sql query to database
 * Input:        @query
 * Output:       None
 * Return:       -1: Error occured
 *                0: Successfully executed
 */
int MySQL::sendQuery(const char* sqlQuery)
{
    // Link MySQL server use specific socket
    if(mysql_real_query(&m_MySQL, sqlQuery, strlen(sqlQuery)))  
    {
        fprintf(stderr, "Failed to send sql query: Error: %s\n", mysql_error(&m_MySQL));
        return -1;
    }
    else
    {
        printf("Successfully send sql query:%s\n", sqlQuery);
        m_Result = mysql_store_result(&m_MySQL);
        return 0;
    }
}

/*
 * Function:     fetchRowData
 * Description:  Fetch one row data from result
 * Input:        @recvBuff: buffer for receiving data
 * Output:       None
 * Return:       -1: Error occured
 *                0: Result empty
 *               >0: # of returned fields 
 */
size_t MySQL::fetchRowData(char** &recvBuff)
{
    size_t  numFields;                  // # of fields in fetched data   
    MYSQL_ROW   rowData;                // Data of a row in result
    long unsigned int* lenArray;        // Byte length in each filed

    // Check MYSQL_RES
    if (m_Result == NULL)
    {
        fprintf(stderr, "No result aviable\n");
        return -1;
    }
    
    numFields = mysql_num_fields(m_Result);
    recvBuff = new char* [numFields];

    // Read in one row from result set
    if ((rowData = mysql_fetch_row(m_Result)))
    {
        lenArray = mysql_fetch_lengths(m_Result);
        // Restore each field of row
        for(size_t i = 0; i < numFields; i++)
        {
            // Field not null allocate space and store data
            if (lenArray[i] != 0)
            {
                recvBuff[i] = new char [lenArray[i]]; 
                sprintf(recvBuff[i], "%s", rowData[i]); 
            }
            // Field has no data set buffer null
            else
            {
                recvBuff[i] = NULL;
            }
        }
        return numFields;
    }
    // NO data in result set
    else
    {        
        fprintf(stderr, "Result set is empty\n");
        return 0;
    }
}

/*
 * Function:     clearBuff
 * Description:  Clear used buffer
 * Input:        @buff: buffer to be cleared
 *               @numFields: # of fields in buff
 * Output:       None
 * Return:       None
 */
void MySQL::clearBuff(char** &buff, size_t numFields)
{
    // Clear buffer used for storing result
    for (size_t i = 0;i < numFields; i++)
    {
        if (buff[i] != NULL)
        {
            delete[] buff[i];
        }
    }
    delete[] buff;
}

/*
 * Function:     disconnect
 * Description:  Close current connect
 * Input:        None
 * Output:       None
 * Return:       None
 */
void MySQL::disconnect()
{
    mysql_close(&m_MySQL);
}
