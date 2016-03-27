/*************************************************
*
*   FileName:       PDFParser.h
*   Author:         WangYun	
*   Date:           Mar.18 2010
*   Description:    Definition of class PDFParser
*
**************************************************/


#ifndef PDFPARSER_H
#define PDFPARSER_H

#include "common.h"
#include <zlib.h>

/*
 * Macro Definition
 */ 
const int DOUBLE_EXTEND = 2;                // Double decompressed buffer size
const int DFEXTENT = 10;                    // Default extent size
const int THRESHOLD = 3000000;              // Maximum decompress size of byte
const size_t LEN_CHAR = 4;                  // Suffix length of char segment
const size_t LEN_RANGE = 5;                 // Suffix length of range segment
const size_t DETECT_RANGE = 1000;           // Detect range of a font object
const size_t USE_EX_FONT = 1;               // Use external font info
const size_t NO_EX_FONT = 0;                // No external font info
const size_t USE_UNICODE = 1;               // Font needs to be mapped to unicode
const size_t NO_UNICODE = 0;                // Font dosen't need to be mapped to unicode
const size_t SEARCH_THRESHOLD = 100;        // Threshold of search length
const int NORMAL_STORE = 0;                 // Objects in PDF stored in normal way
const int STREAM_STORE = 1;                 // Objects in PDF stored in object stream
const int MAX_WIDTH = 4;                    // Max Character code width
const int NO_CMAP = 1;                      // No CMaps contained in PDF
const int USE_CMAP = 2;                     // Translate character code by CMap
const int HEX_CODE = 1;                     // Hex character code
const int LIT_CODE = 2;                     // Literal character code
const string EX_CMAP = "excmap";            // Unified name of external CMap
const int IMG = -1;                         // PDF is pure image
const int NEED_CMAP = -2;                   // PDF needs external CMaps

/*
 * STL Type Definition
 */

// Used to store PDF CMap structure <Character code, UCS2 code>
typedef map<u_short, u_short> MapBody;          
typedef pair<u_short, u_short> MapBodyPair;

// Structure describes a CMap
typedef struct cmap
{
    size_t codeWidth;        // Character code width 
    MapBody mapBody;         // Map body
}CMap;

// Character string (Hex or Literal)
typedef struct char_string
{
    int type;
    string content;
}CharString;

// Union describes a set of literal characters
typedef union literal
{
    u_short code;
    char buffer[2];
}Literal;

// Used to store all CID Maps <Index, CMap>
typedef map<string, CMap> MapsTable;        
typedef pair<string, CMap> MapsTablePair;

// Used to store store Font obj and its CMap obj
// <Font Object Sequence, CMap Object Sequence>
typedef map<string, string> FontMap;       
typedef pair<string, string> FontMapPair;

// <Font Name, CMap Object Sequence>
typedef multimap<string, string> MapInfo;   
typedef pair<string, string> MapInfoPair;   

// Used to store Font quote <Font Name, Font Descriptor Object Sequence>
typedef multimap<string, string> FontQuote;       
typedef pair<string, string> FontQuotePair; 

// Merged CMaps <Font Name, CMap>
typedef multimap<string, CMap> MergedMap;
typedef pair<string, CMap> MergedMapPair;

// Extract result in a text string 
typedef map<int, CharString> ExtractResult; 
typedef pair<int, CharString> ExtractResultPair; 

// Font name list
typedef set<string> FontList;

// Page object sequence list
typedef list<string> PageList;

/*
 * Class: PDFParser
 * Description: Extract text information from PDF
 *
 */
class PDFParser  
{
private:
    /*
     * Attributes
     */
    char*   m_DocBuffer;        // Cacheing PDF file data in the buffer
    char*   m_TempBuffer;       // Caching PDF temp file data in the buffer
    int     m_StoreType;        // Object stored traits, normal or in stream     
    size_t  m_FileLength;       // File length of PDF
    size_t  m_TempLength;       // File length of PDF temp
    string  m_CurFileName;      // Current extracting file name   
    fstream m_FileTemp;         // File temp store inflated stream 
    MapsTable m_CMaps;          // Map character code to unicode   
    FontQuote m_FontQuote;      // Font quote table
    FontMap m_FontMap;          // Sequnce of Font object and its CMap object
    FontList m_FontList;        // List of font names in  current PDF
    PageList m_PageList;        // List of pages of current PDF
    MapInfo m_MapInfo;          // Font name and CMap object which it could use
    MergedMap m_MergedMaps;     // Merged CMaps of same font with same code width
   
public:	
    /*
     * Methods
     */
    //int findLabel(const char* buffer, const char* obj, size_t offset, size_t len); 
    int inflateStream(size_t start, size_t inSize, char*& bufOut, size_t& outSize, int extent); 
    void writeTemp(const char* decode, u_int extend);
    int getCMap(string objSeq, const char* searchBuffer, size_t searchLength); 
    int getFontInfo(const char* searchBuffer, size_t searchLength);
    int getPageInfo();
    void findFontQuote(const char* searchBuffer, size_t searchLength);
    void probeNewPage(string objSeq, const char* searchBuffer, size_t searchLength);
    void handlePage();
    void read(const char* path);    
    void filterEOL(const char* buffer, size_t& start, size_t& end);
    int execExtract();
    int findObject(string objSeq, int& objOffset, size_t& objLen);
    int findObjectStraight(string objSeq, int& objOffset, size_t& objLen);
    int findObjectInStream(string objSeq, int& objOffset, size_t& objLen);    
    int findMapObject(string fontName, string objSeq);
    int findTextString(const char* searchBuffer, size_t searchLength);
    int loadExternalCMap(const char* mapName);
    int mergeCMap(string fontName);
    int getText(string fontName, const char* textStr, size_t textLen);
    
    /*
     * Construction methods
     */
    PDFParser();
    virtual ~PDFParser();
};
#endif 
