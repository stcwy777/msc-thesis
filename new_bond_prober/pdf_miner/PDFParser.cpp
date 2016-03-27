/*************************************************
*
*       FileName:    PDFParser.cpp
*         Author:    WangYun	
*           Date:    Mar.18 2010
*    Description:    APIs implementation of Class 
*                    PDFParser
*
**************************************************/


#include "PDFParser.h"

PDFParser::PDFParser()
{
}

PDFParser::~PDFParser()
{
    /*
    m_UnicodeMap.clear();    
    m_FontQuote.clear();
    m_FontInfo.clear();

    m_FontQuote.clear();
    m_FontList.clear();
    m_FontMap.clear();
    m_MapInfo.clear();
    m_CMaps.clear();
    m_MergedMaps.clear();
    */
} 

/*
 * Function:     readPdf
 * Description:  Read data from PDF file
 * Input:        @path: File path of PDF
 * Output:       None    
 * Return:       None               
 */
void PDFParser::read(const char* path)
{
    string tempPath;        // File path of temporary file
    string textFilePath;    // File path of text file
    ifstream pdfStream;     // File stream for PDF
    fstream textFile;       // File stream fo text file
    
    // Open PDF file
    pdfStream.open(path, ios::in | ios::binary);
    CHECKFILE(pdfStream.is_open(), path);
    
    // Get file size
    pdfStream.seekg(0, ios::end);
    m_FileLength = pdfStream.tellg();
    pdfStream.seekg(0, ios::beg);
    
    // Allocate space for PDFDocBuffer
    m_DocBuffer = new char [m_FileLength];
    // Caching data into buffer
    pdfStream.read(m_DocBuffer, m_FileLength);
    pdfStream.close();

    // Open & wrtie PDF temp file
    m_CurFileName = string(path, strlen(path) - strlen(".pdf"));
    tempPath = m_CurFileName + ".tmp";
    m_FileTemp.open(tempPath.c_str(), ios::in | ios::out | ios::binary | ios::trunc);
    CHECKFILE(m_FileTemp.is_open(), tempPath.c_str());
    writeTemp("FlateDecode", DOUBLE_EXTEND);

    // Clear related text file
    textFilePath =  m_CurFileName + ".txt";
    textFile.open(textFilePath.c_str(), ios::out);
    textFile.close();
}

/*
 * Function:     findObject
 * Description:  Find syntax objects in PDF temporary buffer
 * Input:        @buffer: source string 
 *               @obj: name of object
 *               @len: max length of buffer
 *               @offset: offset of start pos
 * Output:       None 
 * Return:       -1: Not found
 *               >0: Start position of object
 */
/*
int PDFParser::findLabel(const char* buffer, const char* obj, size_t offset, size_t len)
{
    size_t objLength;        // String length of object phrase
    size_t matchedLength;    // Matched length during search
    size_t pos;              // Position of object in buffer

    // Initialize variables before searching	
    objLength = strlen(obj);				
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
    
    // End rotation unable to find
    return -1;
}
*/

/*
 * Function:     inflateStream
 * Description:  Decompression data using ZLIB
 * Input:        @start: start position of buffer
 *               @size: size of decompression
 *               @extent: extent of input size
 * Output:       @bufOutput: buffer of inflated data
 *               @outSize: output size of inflated stream
 * Return:       ot: Error occured
 *               0: Successfully executed
 */
int PDFParser::inflateStream(size_t start, size_t inSize, char*& bufOutput, size_t& outSize, int extent)
{
    /* 
     * Structure defined in zlib DLL
     * For more details check zlib.net
     */
    int z_ret;
    int	z_size;
    z_stream z_strm;

    // Initialize z_stream data according to ZLIB Manual
    memset(&z_strm, 0, sizeof(z_strm));
    z_strm.next_in = (Bytef *)(m_DocBuffer + start);
    z_strm.avail_in = inSize;

    z_strm.avail_out = inSize * extent;
    z_strm.next_out = (Bytef *)bufOutput;

    // Ready for inflate
    z_ret = inflateInit(&z_strm);

    // Initialize successful
    if (z_ret == Z_OK)
    {
        // Entirely decompress
        z_size = inflate(&z_strm, Z_FINISH);

        // Inflate successful
        if (z_size >= 0 || z_size == Z_BUF_ERROR)
        {
            outSize = inSize * extent - z_strm.avail_out;
            return 0;
        }
        // Encoutered error
        else
        {
            /*
             * 'cos size error occured frequently
             * we print this error only if debugging decompress functions
             */
            // Debug:
            //cout << " Can't decompress data " << endl;
            return z_size;
        }
    }
    else
    {
        // Oherwise unable to inflate
        return z_ret;	
    }

}

/*
 * Function:     filterEOL
 * Description:  Filter end-of-line
 * Input:        @buffer: source buffer to filter
 *               @start: start position of buffer
 *               @end:	end position of buffer
 * Output:       None  
 * Return:       None
 */
void PDFParser::filterEOL(const char* buffer, size_t& start, size_t& end)
{
    // Filter end-of-line from beginning
    if (buffer[start] == CR && buffer[start + 1] == LF)
    {
        start += 2;         // EOL = CRLF skip two bytes
    }
    else if (buffer[start] == LF)
    {
        start++;	    // EOL = LF skip one byte					
    }

    // Filter end-of-line before ending (similar as from beginning)
    if (buffer[end - 1] == CR && buffer[end] == LF)
    {
        end -= 2;
    }
    else if (buffer[end] == LF)
    {
        end--;
    }
}

/*
 * Function:     writeTemp
 * Description:  Write inflated stream data to temporary file
 * Input:        @decode: Decode label of needed stream
 *               @extend: enlarge extend while unzipping
 * Output:       None       
 * Return:       None
 */
void PDFParser::writeTemp(const char* decode, u_int extend)
{
    int extent;                             // Decompressed buffer size
    int ret;                                // Return status of inflate function
    char* bufOutput;                        // Point of output buffer
    size_t outSize;                         // Size of outpout buffer
    int offset = 0;                         // File offset
    size_t anchor = 0;                      // Anchor point     
    size_t strmStart, strmEnd, strmSize;    // Stream related value
    size_t endPos;                          // End position of lastest output by searching "FlateDecode"
    
    // Find label "FlateDecode"
    while ((offset = findLabel(m_DocBuffer, "FlateDecode", offset, m_FileLength, FORWARD)) != -1)
    {
        // Search <stream...endstream> pairs
        strmStart = findLabel(m_DocBuffer, "stream", offset, m_FileLength, FORWARD);
        strmEnd = findLabel(m_DocBuffer, "endstream", strmStart, m_FileLength, FORWARD);
        offset = strmEnd + strlen("endstream");
        m_FileTemp.write(m_DocBuffer + anchor, strmStart - anchor);
        anchor = offset;
        m_FileTemp << "stream" << endl;

        // Filter EOL
        strmStart += strlen("stream");
        strmEnd--;
        filterEOL(m_DocBuffer, strmStart, strmEnd);
        strmSize = strmEnd - strmStart + 1;
	    
        // Trying to inflate
        extent = DFEXTENT;
        bufOutput = new char[strmSize * extent];
        ret = inflateStream(strmStart, strmSize, bufOutput, outSize, extent);
        // cout << strmSize << ":" << outSize << endl;
        // Not enough space, enlarge output buffer
        while (ret == Z_BUF_ERROR)
        {
            // Double enlarge output buffer
            extent *= extend;
            delete[] bufOutput;
            // If extended size larger than maxium memory, break off this mission
            /*
            if (strmSize * extent > THRESHOLD)
            {
                break;
            }
            */
            bufOutput = new char[strmSize * extent];
            // Re-decompress compressed stream 
            ret = inflateStream(strmStart, strmSize, bufOutput, outSize, extent);
            //cout << "Enlarge size" << strmSize << endl;
        }
        
        // Encountered error while executing zlib decompress
        if (ret == Z_BUF_ERROR)
        {
            continue;
        }
        else if (ret < 0)
        {
            string errStatement = "Error using zlib functions with return value:" + ret;
            delete[] bufOutput;
            delete[] m_DocBuffer;
            ERROR_ALERT(errStatement.c_str());
        }
        // Output decompressed buffer
        else
        {
            m_FileTemp.write(bufOutput, outSize);
            m_FileTemp << "endstream" << endl;
            delete[] bufOutput;
        }
    }
    
    // Add left of PDF to temp file directly 
    endPos = strmEnd + sizeof("endstream");
    if (m_FileLength > endPos)
    {    
        m_FileTemp.write(m_DocBuffer + endPos, m_FileLength - endPos);
    }
    
    // Clear PDF document buffer
    delete[] m_DocBuffer;
}

/*
 * Function:     execExtract
 * Description:  Execute text extracting process
 * Input:        None
 * Output:       None       
 * Return:       ot: Error occured
 *               0:  Successfully executed
 */
int PDFParser::execExtract()
{
    int fileType;                   // Type of current PDF file
    FontList::iterator fontIter;    // Fonts iterator
    
    // Get temp file size
    m_FileTemp.seekg(0, ios::end);
    m_TempLength = m_FileTemp.tellg();
    m_FileTemp.seekg(0, ios::beg);
    
    // Allocate space for temp file buffer
    m_TempBuffer = new char [m_TempLength];
    
    // Caching data into buffer for further operation
    m_FileTemp.read(m_TempBuffer, m_TempLength);
    m_FileTemp.close();

    // Initialize store type
    m_StoreType = NORMAL_STORE;
    // Get type of current PDF file    
    fileType = getFontInfo(m_TempBuffer, m_TempLength);

    //cout << fileType << endl;
    // Return directly when PDF is of pure image
    if (fileType == IMG)
    {
        cout << "[Warning] Failed extracting image PDF" << endl;
        m_FontQuote.clear();
        m_FontList.clear();
        delete[] m_TempBuffer;
        return -1;
    }
    else if (fileType == NEED_CMAP)
    {
        // Load external tabel
        cout << "[Status] Loading external CMaps..." << endl;
        loadExternalCMap("Adobe-GB1-UCS2");
    }

    /*
     * Extract text directly when PDF needs external CMaps
     * or can't find page information in PDF
     * Otherwise extract text page by page
     */
    
    if (fileType == NEED_CMAP || findLabel(m_TempBuffer, "/Contents", 0, m_TempLength, FORWARD) == -1)
    {
        // Merge maps for fonts
        for (fontIter = m_FontList.begin(); fontIter != m_FontList.end(); fontIter++)
        {
            mergeCMap(*fontIter);
        }

        // Find and extract text
        cout << "[Status] Extracting text directly..." << endl;
        findTextString(m_TempBuffer, m_TempLength); 
    }
    else
    {
        cout << "[Status] Extracting text by page..." << endl;
        
        // Release resources allocated while checking font information
        m_FontQuote.clear();
        m_MapInfo.clear();
        if (getPageInfo() == 0)
        {
            handlePage();
        }
        else
        {
            string errStatement = "Error missing page root";
            ERROR_ALERT(errStatement.c_str());
        }
        
    }
    
    // Release resources
    m_FontQuote.clear();
    m_FontList.clear();
    m_PageList.clear();
    m_FontMap.clear();
    m_MapInfo.clear();
    m_CMaps.clear();
    m_MergedMaps.clear();
    delete[] m_TempBuffer;

    cout << "[Status] Succeed extracting text" << endl;
    return 0;
}

/*
 * Function:     getPageInfo
 * Description:  Extract text from PDF by page
 * Input:        None
 * Output:       None       
 * Return:       ot: Error occured
 *               0:  Successfully executed
 */
int PDFParser::getPageInfo()
{
    pcre* regPageRoot = NULL;       // Regex structure for page root
    int numMatched = 0;                 // Matched # of strings
    int rootOffset;                 // Offset of root object
    int rootMatch[MAX_MATCH];       // Match result of page root
    size_t rootLen;                 // Length of root object
    string rootSeq;                 // Sequence of root object
    const char* rootBuffer;
    const char* subStr = NULL;          // Temp for captured sub string  

    // Find root sequence of pages
    setRegex(&regPageRoot, "(?!ype)[Pp]ages\\s?\\n?(\\d+)\\s+0\\s+R");
    if ((numMatched = pcre_exec(regPageRoot, NULL, m_TempBuffer, m_TempLength,
                                NO_OFFSET, NO_OPTION, rootMatch, MAX_MATCH)) > 0)
    {
        // Capture sequence of page root object
        pcre_get_substring(m_TempBuffer, rootMatch, numMatched, 1, &subStr);
        rootSeq = string(subStr);
        pcre_free_substring(subStr);
        //cout << rootSeq << endl;
        
        // Get buffer point of page root
        if (findObject(rootSeq, rootOffset, rootLen) < 0)
        {
            string errStatement = "Error missing page root object:" + rootSeq;
            ERROR_ALERT(errStatement.c_str());
        }

        // Set page probe buffer & probe new pages
        rootBuffer = m_TempBuffer + rootOffset;
        probeNewPage(rootSeq, rootBuffer, rootLen);
        
        // Check size of page list
        if (m_PageList.size() == 0)
        {
            string errStatement = "Error no page probed";
            ERROR_ALERT(errStatement.c_str());
        }
    }
    else
    {
        pcre_free(regPageRoot);
        return -1;
    }
    
    pcre_free(regPageRoot);
    return 0;
}

/*
 * Function:     handlePage
 * Description:  Extract text from PDF by page
 * Input:        None
 * Output:       None       
 * Return:       ot: Error occured
 *               0:  Successfully executed
 */
void PDFParser::handlePage()
{
    pcre* regContents = NULL;           // Regex structure for page Contents label
    pcre* regObject = NULL;             // Regex structure for object reference
    pcre* regObjList = NULL;            // Regex structure for object list
    pcre* regRsrcsObject = NULL;        // Regex structure for Resources object
    int contOffset = 0;                 // Offset of captured "Contents" object
    int rsrcsOffset = 0;                // Offset of resources' content
    int objOffset = 0;                  // Offset of object
    int pageOffset;                     // Offset of page descriptor
    int contLabelMatch[MAX_MATCH];      // Match result of label "Contents"
    int objListMatch[MAX_MATCH];        // Match result of object lists
    int rsrcsMatch[MAX_MATCH];          // Resources matched result
    int objMatch[MAX_MATCH];            // Object matched result
    int numMatched = 0;                 // Matched # of strings
    string pageSeq;                     // Sequence of page object
    string contSeq;                     // Sequence of Contents
    string rsrcsSeq;                    // Sequence of Resources
    size_t contLen;                     // Length of captured "Contents"
    size_t rsrcsLen;                    // Length of Resources object   
    size_t pageLen;                     // Length of page object
    size_t searchLen;                   // Search buffer length   
    const char* pageBuffer;             // Buffer point of page descriptor
    const char* subStr = NULL;          // Temp for captured sub string
    const char* searchBuffer;           // Search buffer point
    FontQuote::iterator quoteIter;      // Iterator of Character to Unicode map    
    PageList::iterator pageIter;        // Iterator of page list
    
    // Set pcre regex structure
    setRegex(&regContents, "/Contents\\s(\\d+)\\s0\\sR");
    setRegex(&regRsrcsObject, "(Resources)\\s(\\d+)\\s0\\sR");  
    setRegex(&regObjList, "\\[((\\s?\\d+\\s0\\sR\\s?)+)\\]");
    setRegex(&regObject, "(\\d+)\\s0\\sR");

    // Handle page by page
    for (pageIter = m_PageList.begin(); pageIter != m_PageList.end(); pageIter++)
    {
        pageSeq = *pageIter;
        // Locate page object
        if (findObject(pageSeq, pageOffset, pageLen) < 0)
        {
            string errStatement = "Error missing page object:" + pageSeq;
            ERROR_ALERT(errStatement.c_str());
        }
        
        pageBuffer = m_TempBuffer + pageOffset;
        
        // Find Contents object of this page
        if ((numMatched = pcre_exec(regContents, NULL, pageBuffer, pageLen,
                                    NO_OFFSET, NO_OPTION, contLabelMatch, MAX_MATCH)) > 0)
        {   
            // Capture Contents sequence
            pcre_get_substring(pageBuffer, contLabelMatch, numMatched, 1, &subStr);
            contSeq = string(subStr);
            pcre_free_substring(subStr);
            
            // Resources of this page contained in another object
            if ((numMatched = pcre_exec(regRsrcsObject, NULL, pageBuffer, pageLen,
                                        NO_OFFSET, NO_OPTION, rsrcsMatch, MAX_MATCH)) > 0)
            {
                pcre_get_substring(pageBuffer, rsrcsMatch, numMatched, 2, &subStr);
                rsrcsSeq = string(subStr);
                pcre_free_substring(subStr);
                
                //cout << rsrcsSeq << "..." << endl;
                // Handle two cases:objected stored in normal form or stored in object stream
                if (findObject(rsrcsSeq, rsrcsOffset, rsrcsLen) < 0)
                {
                    string errStatement = "Error missing inner object resources:" + rsrcsSeq;
                    ERROR_ALERT(errStatement.c_str());
                }
                else
                {
                    // Find object then Get CMap information
                    getFontInfo(m_TempBuffer + rsrcsOffset, rsrcsLen);
                }
            }
            // Resources contained in page object directly
            else
            {
                // Find font information in current range
                getFontInfo(pageBuffer, pageLen);
            }
            
            // If can't find font information in Resources skip current Page
            if (m_FontQuote.size() == 0)
            {
                continue;
            }
            // Otherwise merge CMaps
            else
            {    
                for (quoteIter = m_FontQuote.begin(); quoteIter != m_FontQuote.end(); quoteIter++)
                {
                    //cout << quoteIter->first << endl;
                    mergeCMap(quoteIter->first);
                }
            }
            
            // Check if current Contents have text 
            findObjectStraight(contSeq, contOffset, contLen);
            
            // Set search buffer & length for object list
            searchBuffer = m_TempBuffer + contOffset;
            searchLen = contLen;
            
            // Check whether contents of this are a object list
            if ((numMatched = pcre_exec(regObjList, NULL, searchBuffer, searchLen,
                                        NO_OFFSET, NO_OPTION, objListMatch, MAX_MATCH)) > 0)
            {   
                // Set search buffer & length for object in object list
                searchBuffer = searchBuffer + objListMatch[2];
                searchLen = objListMatch[3] - objListMatch[2];
                //cout << string(searchBuffer, searchLen) << endl;
                
                // Find all objects in object list
                objOffset = 0;
                while ((numMatched = pcre_exec(regObject, NULL, searchBuffer, searchLen,
                                                   objOffset, NO_OPTION, objMatch, MAX_MATCH)) > 0)
                {
                    pcre_get_substring(searchBuffer, objMatch, numMatched, 1, &subStr);
                    contSeq = string(subStr);
                    pcre_free_substring(subStr);
                    //cout << "@@" << contSeq << endl;
                    
                    findObjectStraight(contSeq, contOffset, contLen);
                    findTextString(m_TempBuffer + contOffset, contLen);
                    
                    objOffset = objMatch[1];
                }
                
            }
            else
            {
                // Try to find text contents of this "Contents" object
                findTextString(m_TempBuffer + contOffset, contLen);
            }
            
            // Clear font & map resources for next search
            m_FontQuote.clear();
            m_MapInfo.clear();
            m_MergedMaps.clear();
        }
    }
    
    // Clear prece object
    pcre_free(regContents);
    pcre_free(regRsrcsObject);
    pcre_free(regObjList);
    pcre_free(regObject);;
}

/*
 * Function:     probeNewPage
 * Description:  Extract text from PDF by page
 * Input:        objSeq: object sequence of current root
 *               searchBuffer: buffer address to search new pages
 *               searchLength: buffer length
 * Output:       None       
 * Return:       None
 */
void PDFParser::probeNewPage(string objSeq, const char* searchBuffer, size_t searchLength)
{
    pcre* regKids;                  // Regex for kids in a page root object
    pcre* regObject;                // Regex for an object
    int numMatched = 0;             // Matched # of strings
    int objOffset;                  // Offset of object
    int kidOffset;                  // Offset of captured kid
    int kidListOffset = 0;          // Offset of a kid list
    int kidListMatch[MAX_MATCH];    // Match result of kid list
    int objMatch[MAX_MATCH];        // Match result of object
    string kidSeq;                  // Sequence of captured kid
    size_t kidLength;               // Length of kid object
    size_t kidListLength;           // Length of kid list 
    const char* kidListBuffer;      // Point of kid list buffer
    const char* kidBuffer;          // Point of kid object buffer
    const char* subStr = NULL;      // Temp for captured sub string
    
    // Set pcre regext
    setRegex(&regKids, "Kids(((?!\\]).)+)\\]");
    setRegex(&regObject, "(\\d+)\\s0\\sR");
    
    // Missing label "Kids" means current object is a page descriptor
    if ((numMatched = pcre_exec(regKids, NULL, searchBuffer, searchLength,
                                 kidListOffset, NO_OPTION, kidListMatch, MAX_MATCH)) > 0)
    {
        //pcre_get_substring(searchBuffer, kidsMatch, numMatched, 1, &subStr);
        // Set buffer point & length of kids
        kidListBuffer = searchBuffer + kidListMatch[2];
        kidListLength = kidListMatch[3] - kidListMatch[2];

        objOffset = 0;
        
        // Find kids of this page
        while ((numMatched = pcre_exec(regObject, NULL, kidListBuffer, kidListLength,
                                       objOffset, NO_OPTION, objMatch, MAX_MATCH)) > 0)
        {
            pcre_get_substring(kidListBuffer, objMatch, numMatched, 1, &subStr);
            kidSeq = string(subStr);
            pcre_free_substring(subStr);

            // Locate current kid object
            if (findObject(kidSeq, kidOffset, kidLength) < 0)
            {
                string errStatement = "Error missing kids object:" + kidSeq;
                ERROR_ALERT(errStatement.c_str());               
            }

            // Set buffer point of this kid
            kidBuffer = m_TempBuffer + kidOffset;

            // Recurrence probe new pages
            probeNewPage(kidSeq, kidBuffer, kidLength);

            objOffset = objMatch[1];
        }
        
    }
    else
    {
        // Push this page into list
        m_PageList.push_back(objSeq);
    }
    
    // Release resources
    pcre_free(regKids);
    pcre_free(regObject);
}

/*
 * Function:     getFontInfo
 * Description:  Get font information in specific buffer area
 * Input:        searchBuffer: buffer area to search
 *               searchLength: buffer area length
 * Output:       None       
 * Return:       ot: Error occured
 *               0:  Successfully executed
 */
int PDFParser::getFontInfo(const char* searchBuffer, size_t searchLength)
{
    pcre* regQuoteSeg;              // Regex structure for font quote segment
    pcre* regQuoteObj;              // Regex structure of font quote object
    pcre* regQuote;                 // Regex structure compiled for font Quote
    string quoteSeg;                // String of fonts quote segment
    string fontName;                // String of font name
    string objSeq;                  // Sequence of object   
    int quoteSegMatch[MAX_MATCH];   // Quote segment matched result
    int quoteObjMatch[MAX_MATCH];   // Quote object matched result
    int quoteSegOffset = 0;         // Offset of quote segments matching
    int quoteObjOffset = 0;         // Offset of quote object matching
    int numMatched = 0;             // Matched # of strings
    int objOffset;                  // Object offset 
    size_t objLen;                  // Object Length
    const char* subStr = NULL;      // Temp for captured sub string
    FontQuote::iterator  quoteIter; // Iterator of Character to Unicode map    

    
    // Iterator range of quote of some font
    pair<FontQuote::iterator, FontQuote::iterator> retRange;
    
    // Set pcre regex 
    setRegex(&regQuoteSeg, "Font\\n?\\s?<<([^>><<]*)\\n?>>");
    //setRegex(&regQuoteSeg, "/Font\\n?\\s?<<");
    setRegex(&regQuoteObj, "Font\\s*(\\d+)\\s*0\\s*R");
    setRegex(&regQuote, "/([^/]*)\\s(\\d{1,6})\\s0\\sR");

    /*
     * Searching quote info to find font descripter object
     * We store font name and object sequence FontQuote
     */
    
    // Rotate if still could match new font quote
    while ((numMatched = pcre_exec(regQuoteSeg, NULL, searchBuffer, searchLength, 
                                   quoteSegOffset, NO_OPTION, quoteSegMatch, MAX_MATCH)) > 0)
    {
        // Capture quote segmengt contained information we need
        pcre_get_substring((const char*)searchBuffer, quoteSegMatch, numMatched, 1, &subStr);
        quoteSeg = string(subStr);
        pcre_free_substring(subStr);
        //endobjPos = findLabel(searchBuffer, ">>", quoteSegMatch[1], searchLength);
        //objLen = endobjPos - quoteSegMatch[1] + strlen(">>");
        //cout << quoteSeg << endl;
        findFontQuote(quoteSeg.c_str(), quoteSeg.size());
        //findFontQuote(searchBuffer + quoteSegMatch[1], objLen);

        // Match next segment
        quoteSegOffset = quoteSegMatch[1];
    }
    
    // Can't find any font quote directly try to find quotes in object
    if (m_FontQuote.size() == 0)
    {
        //cout << string(searchBuffer, searchLength) << endl;
        // Rotate if could find any font quote object
        while ((numMatched = pcre_exec(regQuoteObj, NULL, searchBuffer, searchLength, 
                                       quoteObjOffset, NO_OPTION, quoteObjMatch, MAX_MATCH)) > 0)
        {
            pcre_get_substring((const char*)searchBuffer, quoteObjMatch, numMatched, 1, &subStr);
            objSeq = string(subStr);
            pcre_free_substring(subStr);
            // Find this object
            if ( findObjectStraight(objSeq, objOffset, objLen) != -1)
            {
                // Then locate font quote of this font object
                findFontQuote(m_TempBuffer + objOffset, objLen);
            }
            // Match next
            quoteObjOffset = quoteObjMatch[1];
        }
        
    }
    
    // Couldn't find any font quote in both format
    if (m_FontQuote.size() == 0)
    {
        cout << "[Warning] Missing match font quote" << endl;
        pcre_free(regQuoteSeg);
        pcre_free(regQuote);       
        return -1;
    }
    
    // For each quoted font find its font descriptor object
    for (quoteIter = m_FontQuote.begin(); quoteIter != m_FontQuote.end(); quoteIter++)
    {
        //cout << quoteIter->first << " " << quoteIter->second  << " " << endl;
        fontName = quoteIter->first;
        objSeq = quoteIter->second;
        
        // Find object then Get CMap information
        findMapObject(fontName, objSeq);
    } 
    
    // Release resources
    pcre_free(regQuoteSeg);
    pcre_free(regQuote);

    // Parser needs external CMaps
    if (m_CMaps.size() == 0)
    {
        return NEED_CMAP;
    }
    else
    {    
        return 0;
    }
    
}

/*
 * Function:     findFontQuote
 * Description:  Try to find font quote information in buffer
 * Input:        searchBuffer: buffer area to search
 *               searchLength: buffer area length
 * Output:       None       
 * Return:       None
 */
void PDFParser::findFontQuote(const char* searchBuffer, size_t searchLength)
{
    pcre* regQuote;                 // Regex structure compiled for font Quote
    string fontName;                // String of font name
    string objSeq;                  // Sequence of object   
    int quoteMatch[MAX_MATCH];      // Quote matched result
    int quoteOffset = 0;            // Offset of quote matching
    int numMatched = 0;             // Matched # of strings
    const char* subStr = NULL;      // Temp for captured sub string
    FontQuote::iterator  quoteIter; // Iterator of Character to Unicode map    

     // Iterator range of quote of some font
    pair<FontQuote::iterator, FontQuote::iterator> retRange;
    
    // Set regex for font quote
    setRegex(&regQuote, "/([^/\\n\\s]*)\\n?\\s?(\\d{1,6})\\s0\\sR");

    // Matching each font quote in current segment
    while ((numMatched = pcre_exec(regQuote, NULL, searchBuffer, searchLength, 
                                   quoteOffset, NO_OPTION, quoteMatch, MAX_MATCH)) > 0)        
    {
        
        // Extract font index part
        pcre_get_substring(searchBuffer, quoteMatch, numMatched, 1, &subStr);
        fontName = string(subStr); 
        pcre_free_substring(subStr);
        
        // Extract quoted object sequence
        pcre_get_substring(searchBuffer, quoteMatch, numMatched, 2, &subStr);       
        objSeq = string(subStr);
        pcre_free_substring(subStr);       
        
        // Insert into font list with unique name
        if (m_FontList.find(fontName) == m_FontList.end())
        {
            m_FontList.insert(fontName);
        }
        
        // Find exists font & quote pair if not repeat then insert this pair
        retRange = m_FontQuote.equal_range(fontName);
        for (quoteIter = retRange.first; quoteIter != retRange.second; quoteIter++)
        {
            if (quoteIter->second == objSeq)
            {
                break;
            }
        }
        
        // No dunplication insert this font quote
        if (quoteIter == retRange.second)
        {
            m_FontQuote.insert(FontQuotePair(fontName, objSeq));
        }
        // Match next font quote 
        quoteOffset = quoteMatch[1];
    }
}

/*
 * Function:     findObject
 * Description:  Find object and try both ways
 * Input:        objSeq: object sequence
 * Output:       objOffset: object offset in temp buffer
 *               objLen: object size
 * Return:       ot: Error occured
 *               0:  Successfully executed
 */
int PDFParser::findObject(string objSeq, int& objOffset, size_t& objLen)
{
    int numAttempt = 0;    // # Attempt to find

    while (numAttempt < 2)
    {
        // Find font descriptor obejct in normal way
        if (m_StoreType == NORMAL_STORE)
        {
            // If can't find object change method
            if (findObjectStraight(objSeq, objOffset, objLen) < 0)
            {
                numAttempt++;
                m_StoreType = STREAM_STORE;
            }
            else
            {                    
                break;
            }
        }
        // Find font descriptor object in object stream
        else if (m_StoreType == STREAM_STORE)
        {
            if (findObjectInStream(objSeq, objOffset, objLen) < 0)
            {
                numAttempt++;
                m_StoreType = NORMAL_STORE;
            }
            else
            { 
				break; 
            }
        }
    }
    
    // Can't find object in both methods
    if (numAttempt >= 2)
    {
        return -1;
    }
    // Find aim object
    else
    {
        return 0;
    }
}

/*
 * Function:     findObjInStream
 * Description:  Find object in object stream
 * Input:        objSeq: object sequence
 * Output:       objOffset: object offset in temp buffer
 *               objLen: object size
 * Return:       ot: Error occured
 *               0:  Successfully executed
 */
int PDFParser::findObjectInStream(string objSeq, int& objOffset, size_t& objLen)
{
    pcre* regObjStream;             // Regex compiled for object Stream
    pcre* regObject;                // Regex compiled for object  
    string stmPattern;              // Pattern of object stream
    string objPattern;              // Pattern of object in a object stream
    int stmMatch[MAX_MATCH];        // Object stream matched result
    int stmOffset = 0;              // Offset of stream matching
    int numMatched = 0;             // Matched # of strings
    int findResult = -1;            // Default missing match
    const char* subStr;             // Sub matched string
    size_t count;                   // Matched count in object stream
    stringstream objList;           // Objects list in object stream 
    string subObj;                  // Sub object in object stream may be obj seq or offset
    string tmpStr;                  // Temp string

    /*
     * Handle stream case
     * 1) Find those object streams contain target object sequence
     * 2) Check matched number is sequence or just a offset
     * 3) Find Font map object in object stream 
     */ 
    // DEBUG: 2011/06/15: revised REGULAR EXPRESSION
    // OLD VERSION: "ObjStm\\n?>>\\n?stream\\n?([^<]*\\s" + objSeq + "\\s[^<]+)"
	// ADD BOARD CONSTRAINT: no '['
    stmPattern = "ObjStm\\n?>>\\n?stream\\n?([^<]*\\s" + objSeq + "\\s[^<[]+)";
    setRegex(&regObjStream, stmPattern.c_str());

    objPattern = "<<([^<]+)>>";
    setRegex(&regObject, objPattern.c_str());
    
    // Find object stream
    while ((numMatched = pcre_exec(regObjStream, NULL, m_TempBuffer, m_TempLength, 
                                   stmOffset, NO_OPTION, stmMatch, MAX_MATCH)) > 0)
    {

        // Capture object list of current stream
        pcre_get_substring((const char*)m_TempBuffer, stmMatch, numMatched, 1, &subStr);
        objList << subStr; 
        pcre_free_substring(subStr);                 
        
        // Find object sequence in stream
        count = 0;
        while (objList)
        {
            objList >> subObj;
            
            // Check matched object is sequence or offset
            if (subObj == objSeq)
            {
                /*
                 * If matched number is object sequence
                 * find its body between <<..>> pair
                 */
                if ((count % 2) == 0)
                {
                    //cout << subObj << endl;
                    objList >> tmpStr;
                    objOffset = atoi(tmpStr.c_str());
                    //cout <<  "@"<< objOffset << endl;

                    // Skip next object
                    tmpStr = "";
                    objList >> tmpStr;
					//cout << "%" << tmpStr << endl;
                    if (tmpStr.size() > 0)
                    {
                        // Calc object length
                        objList >> tmpStr;
                        objLen = atoi(tmpStr.c_str()) - objOffset + 1;
                        //cout << "#"<<tmpStr <<endl;
                    }
                    else
                    {
                        //cout << stmMatch[1] + objOffset << " " << m_TempLength << endl; 
                        objLen = findLabel(m_TempBuffer, "endstream", stmMatch[1] + objOffset, m_TempLength, FORWARD);
                        objLen = objLen - (stmMatch[1] + objOffset);
                        //cout << "111"<<endl;
                    }
                    //cout << objLen << endl;
                    objOffset = stmMatch[1] + objOffset + 1;
                    findResult = 0;
                    break;
                }
            }
            if (findResult != -1)
            {
                break;
            }
            count++;
        }
        // Clear object list and trying to match next object stream
        objList.clear();
        stmOffset = stmMatch[1];
    }
    
    // Release resources
    pcre_free(regObjStream);
    pcre_free(regObject);
    
    // Can't find in stream
    return findResult;
}

/*
 * Function:     findObjectStraight
 * Description:  Find object straight
 * Input:        objSeq: object sequence
 * Output:       objOffset: object offset in temp buffer
 *               objLen: object size
 * Return:       ot: Error occured
 *               0:  Successfully executed
 */
int PDFParser::findObjectStraight(string objSeq, int& objOffset, size_t& objLen)
{
    pcre* regObject;                // Regex compiled for object
    string objPattern;              // Pattern of object in a object stream
    int objMatch[MAX_MATCH];        // Font object matched result
    int endobjPos;                  // Position of end object
    int numMatched = 0;             // Matched # of strings
    int findResult = -1;            // Default missing match

    /*
     * Font oject stored in PDF directly
     */ 
    //cout << "***" << objSeq << endl;
    //objPattern = "[^\\d]" + objSeq + "\\s0\\sobj\\x0d?\\x0a?\\n?";    
    //objPattern = "[^\\d]" + objSeq + "\\s0\\sobj";    
    objPattern = objSeq + "\\s0\\sobj";    
    setRegex(&regObject, objPattern.c_str());
    //regStudy = pcre_study(regObject, NO_OPTION, &errMsg);
    objOffset = 0;
    
    // Find object directly
    while ((numMatched = pcre_exec(regObject, NULL, m_TempBuffer, m_TempLength, 
                                   objOffset, NO_OPTION, objMatch, MAX_MATCH)) > 0)
    {
        if (m_TempBuffer[objMatch[0] - 1] <= 57 && m_TempBuffer[objMatch[0] - 1] >= 48)
        {
            objOffset = objMatch[1];
            continue;
        }
        objOffset = objMatch[1];
        endobjPos = findLabel(m_TempBuffer, "endobj", objOffset, m_TempLength, FORWARD);
        objLen = endobjPos - objOffset + sizeof("endobj");
        //cout<< objLen << endl;
        findResult = 0;
        break;
    }
    // Release resource
    pcre_free(regObject);
    return findResult;
}

/*
 * Function:     findMapObject
 * Description:  Find  object for CMap
 * Input:        fontName: font name 
 *               objSeq: font quoted object
 * Output:       None       
 * Return:       ot: Error occured
 *               0:  Successfully executed
 */
int PDFParser::findMapObject(string fontName, string objSeq)
{
    pcre* regMap;                   // Regex compiled for map label
    string mapPattern;              // Pattern of map object
    string mapSeq;                  // Sequence of map object
    int mapObjOffset;               // Offset of CMap object
    size_t mapObjLength;            // Length of CMap object
    int mapMatch[MAX_MATCH];        // Map label matched result
    int numMatched = 0;             // Matched # of strings
    const char* subStr;             // Sub matched string
    FontMap::iterator fontIter;     // Font iterator      
    int fontType;                   // Font type flag
    int objOffset;                  // Offset of font descriptor object 
    size_t objLen;                  // Length of font descriptor object

    // Check dunplication
    fontIter = m_FontMap.find(objSeq);
    if (fontIter != m_FontMap.end())
    {
        //cout << "Having this CMap" << endl;
        if (fontIter->second == "NULL")
        {
            fontType = 0;
        }
        else
        {
            fontType = 1;
            m_MapInfo.insert(MapInfoPair(fontName, fontIter->second));
        }
        return fontType;
    }
    
    // Capture CMap object sequence by label "ToUnicode"
    mapPattern = "ToUnicode\\s+(\\d+)\\s+0[^/]R";
    setRegex(&regMap, mapPattern.c_str());
    
    // Handle two cases:objected stored in normal form or stored in object stream
    if (findObject(objSeq, objOffset, objLen) < 0)
    {
        string errStatement = "Error missing font descriptor object:" + objSeq;
        ERROR_ALERT(errStatement.c_str());
    }
    else
    {
        //cout << objOffset << " " << objLen << endl;
    	//cout << objSeq << endl;
    }
    // Try to find ToUnicode field
    if ((numMatched = pcre_exec(regMap, NULL, m_TempBuffer + objOffset, objLen,
                                0, NO_OPTION, mapMatch, MAX_MATCH)) > 0)
    { 
        // Capture CMap object sequence
        pcre_get_substring((const char*)(m_TempBuffer + objOffset), mapMatch, numMatched, 1, &subStr); 
        //cout << objSeq << "Map:" << subStr << endl;
        mapSeq = string(subStr);
        pcre_free_substring(subStr);
        
        // If succeed finding CMap insert fontSeq and mapSeq into FontMap
        if (findObjectStraight(mapSeq, mapObjOffset, mapObjLength) != -1 &&
            getCMap(mapSeq, m_TempBuffer + mapObjOffset, mapObjLength) == 0)
        {
            //cout << string(m_TempBuffer + offset + mapMatch[0] - 50, 100) << endl;
            m_FontMap.insert(FontMapPair(objSeq, mapSeq));
            m_MapInfo.insert(MapInfoPair(fontName, mapSeq));
        }
        else
        {
            cout << "Error get CMap " << mapSeq << " of font object " << objSeq << endl; 
        }
        fontType = 1;
    }
    else
    {
        // Simple Font
		m_FontMap.insert(FontMapPair(objSeq, "NULL"));
        //cout << "Simple font:" << objSeq << endl;
        fontType = 0;
    }

    pcre_free(regMap);
    return fontType;
}


/*
 * Function:     getCMap
 * Description:  Get CID map information for Unicode mapping from PDF temp file
 * Input:        objSeq: CMap object sequence
 *               searchBuffer: buffer area for searching
 *               searchLength: length of buffer area
 * Output:       None       
 * Return:       ot: Error occured
 *               0:  Successfully executed
 */
int PDFParser::getCMap(string objSeq, const char* searchBuffer, size_t searchLength)
{
    pcre* regObject;                // Regex compiled for object  
    string objPattern;              // Pattern of object in a object stream
    pcre* regHead;                  // Regex structure compiled for map header
    pcre* regChar;                  // Regex structure for beginbfchar
    pcre* regRange[2];              // Regex structure for beginbfrange
    pcre* regCode;                  // Regex structure for 
    u_short charCode;               // PDF inner Character code
    u_short startCode;              // Start code of range segment
    u_short endCode;                // End code of range segment
    u_short ucsCode;                // UCS-2 code
    int headMatch[MAX_MATCH];       // Head match result
    int headOffset = 0;             // Head match offset
    int bodyMatch[MAX_MATCH];       // Body match result
    int bodyOffset;                 // Body match offset
    int codeMatch[MAX_MATCH];       // Code match result for bfrange
    int codeOffset = 0;             // Code match offset
    int numMatched = 0;             // Matched # of strings
    size_t numMaps;                 // # of maps in a segment   
    const char* subStr = NULL;      // Temp for extracted sub string
    const char* codeBuffer;         // Buffer contains code of bfrange
    CMap curCMap;                   // Current Building CMap 
    size_t codeWidth = 0;           // codeWidth of CMap character code
    MapBody::iterator  mapIter;     // Iterator of Character to Unicode map
    
    
    //Find object directly
    //objPattern = "[^\\d]" + objSeq + "\\s0\\sobj\\n?([^endobj]+)endcmap";
    objPattern = "[^\\d]" + objSeq + "\\s0\\sobj\\n?";
    setRegex(&regObject, objPattern.c_str());  

    
    // Set regex for map segment header
    //setRegex(&regHead, "^([0-9]+)\\s+beginbf([a-z]{4,5})$");
    setRegex(&regHead, "([0-9]+)\\s+beginbf([a-z]{4,5})");
    // Set regex for map body "bfchar" & "bfrange"
    setRegex(&regChar, "<(\\w{1,4})>\\s*<(\\w{4})>");
    //setRegex(&regBody[1], "^<(\\w{1,4})>\\s<(\\w{1,4})>\\s<(\\w{1,4})>");
    setRegex(&regRange[0], "<(\\w{1,4})>\\s*<(\\w{1,4})>\\s*(\\[?((?!^)<\\w{4}>\\s*)+\\]?)");
    setRegex(&regCode, "<(\\w{4})>");
    
    // Find CMap object by its sequence
    { 

        // Mactch map body bfchar or bfrange
        while ((numMatched = pcre_exec(regHead, NULL, searchBuffer, searchLength, 
                                       headOffset, NO_OPTION, headMatch, MAX_MATCH)) > 0)
        {
            //cout << " " << numMatched << endl;
            
            // Capture # of maps in a segment return length
            pcre_get_substring((const char*)searchBuffer, headMatch, numMatched, 1, &subStr);
            numMaps = atoi(subStr);                     // atoi marked may be over flow
            pcre_free_substring(subStr);
            //cout << numMaps << endl;
            
            // Capture segment type
            pcre_get_substring((const char*)searchBuffer, headMatch, numMatched, 2, &subStr);       

            // Offset for inner body matching
            bodyOffset = headMatch[1];
            
            // Segment of "beginbfchar"
            if ((size_t)strlen(subStr) == LEN_CHAR)
            {
                //cout << "char" << endl;
                for (size_t i = 0; i < numMaps; i++)
                {
                    if ((numMatched = pcre_exec(regChar, NULL, searchBuffer, searchLength, 
                                                bodyOffset, NO_OPTION, bodyMatch, MAX_MATCH)) > 0)
                    {   
                        // Capture character code
                        pcre_get_substring((const char*)searchBuffer, bodyMatch, numMatched, 1, &subStr);
                        //sprintf((char*)charCode.c_str(), "%s", subStr);                 
                        charCode = hex2int(subStr);
                        if (codeWidth == 0)
                        {
                            codeWidth = strlen(subStr);
                        }
                        else
                        {
                            if (codeWidth != strlen(subStr))
                            {
                                cout << "incoherence code width in char of " << objSeq << endl;
                            }
                        }
                        //cout << charCode << " ";
                        pcre_free_substring(subStr);
                
                        // Capture UCS-2 code
                        pcre_get_substring((const char*)searchBuffer, bodyMatch, numMatched, 2, &subStr);       
                        //printf("%s\n", subStr);   
                        ucsCode = hex2int(subStr);
                        pcre_free_substring(subStr);   
                        
                        // Insert into map
                        curCMap.mapBody.insert(MapBodyPair(charCode, ucsCode)); 
                        // Next match
                        bodyOffset = bodyMatch[1];               
                    }
                }
            }
            // Segment of "beginbfrange"
            else if ((size_t)strlen(subStr) == LEN_RANGE)
            {
                for (size_t i = 0; i < numMaps; i++)
                {
                    if ((numMatched = pcre_exec(regRange[0], NULL, searchBuffer, searchLength, 
                                                bodyOffset, NO_OPTION, bodyMatch, MAX_MATCH)) > 0)
                    {   
                        // Extract start character code
                        pcre_get_substring((const char*)searchBuffer, bodyMatch, numMatched, 1, &subStr);
                     
                        startCode = hex2int(subStr);                    
                        
                        // Get code width
                        if (codeWidth == 0)
                        {
                            codeWidth = strlen(subStr);
                        }
                        else
                        {
                            if (codeWidth != strlen(subStr))
                            {
                                cout << "incoherence code width in range of " << objSeq << endl;
                            }
                        }                        
                        //cout << charCode << " ";
                        pcre_free_substring(subStr);

                        // Capture endcode of code range
                        pcre_get_substring((const char*)searchBuffer, bodyMatch, numMatched, 2, &subStr);       
                        endCode = hex2int(subStr);                      
                        pcre_free_substring(subStr);   
                    
                        // Capture mapped code string
                        pcre_get_substring((const char*)searchBuffer, bodyMatch, numMatched, 3, &subStr);       
                        
                        // Mapped by a start value
                        if (subStr[0] != '[')
                        {
                            // Get start UCS code value
                            ucsCode = hex2int(string(subStr + 1, 4).c_str());
                            pcre_free_substring(subStr);                      
                            
                            // Insert into map by increasing start value
                            for (u_short j = startCode; j <= endCode;)
                            {
                                curCMap.mapBody.insert(MapBodyPair(j, ucsCode));
                                ucsCode++;
                                if (++j == 0)
                                	break;
                            }
                        }
                        else
                        {
                            pcre_free_substring(subStr);
                            
                            // Set code buffer
                            codeBuffer = searchBuffer + bodyMatch[6];
                            codeOffset = 0;
                            
                            // Get code one by one from code buffer
                            for (u_short j = startCode; j <= endCode;)
                            {
                                numMatched = pcre_exec(regCode, NULL, codeBuffer, bodyMatch[7] - bodyMatch[6],
                                                       codeOffset, NO_OPTION, codeMatch, MAX_MATCH);
                                
                                // Capture UCS code & insert into map
                                pcre_get_substring(codeBuffer, codeMatch, numMatched, 1, &subStr);
                                ucsCode = hex2int(subStr);
                                curCMap.mapBody.insert(MapBodyPair(j, ucsCode));
                                
                                // For next match
                                pcre_free_substring(subStr);
                                codeOffset = codeMatch[1];
                                if (++j == 0)
                                	break;                                
                            }
                            

                        }
                        
                        // Next match
                        bodyOffset = bodyMatch[1];               
                    }
                } 
            }

            // Match next bfsegment 
            headOffset = bodyOffset;
            //cout << headOffset << endl;
        }
    
    }
    // Build CID map
    //for (mapIter = curCMap.mapBody.begin(); mapIter != curCMap.mapBody.end(); mapIter++)
    {
         //cout << mapIter->first << " " << mapIter->second << endl;
    }

    // Clear space
    pcre_free(regObject);
    pcre_free(regHead);
    pcre_free(regChar);
    pcre_free(regRange[0]);
    pcre_free(regCode);
    
    // Check if CMap has content
    if (curCMap.mapBody.size() > 0)
    {
        curCMap.codeWidth = codeWidth; 
        m_CMaps.insert(MapsTablePair(objSeq, curCMap));
        return 0;
    }
    else
    {
        cout << "Error:Can't find CMap of object sequence " << objSeq << endl;
        return -1; 
    }
}

/*
 * Function:     findTextString
 * Description:  Find text string in PDF
 * Input:        searchBuffer: buffer area to search text string
 *               searchLength: length of buffer
 * Output:       None       
 * Return:       ot: Error occured
 *               0:  Successfully executed
 */
int PDFParser::findTextString(const char* searchBuffer, size_t searchLength)
{
    pcre* regFontName;              // Regex structure compiled for font name
    pcre* regTextString;            // Regex structure compiled for text string in PDF
    pcre* regEndStream;             // Regex structure for border search
    int fontMatch[MAX_MATCH];       // Font name match result
    int fontOffset = 0;             // Font name match offset
    int contextMatch[MAX_MATCH];    // Context match result
    int textMatch[MAX_MATCH];       // Text string match result
    int textOffset = 0;             // Text string match offset
    int numMatched = 0;             // # of matched strings 
    size_t plusPos;                 // Position of "+" in a string
    const char* subStr = NULL;      // Temp of extracted sub string
    string textPattern;             // Regex pattern of text content
    string fontName;                // Font name of current text string
    string overlap;                 // string for  skipping overlap
    const char* textBuffer = NULL;  // Text buffer
    const char* contextBuffer = NULL;     // Context buffer contains text string
    size_t textSize;                // Buffer size of text
    int contextSize;                // Buffer size of context
    FontList::iterator  fontIter;   // Iterator of Character to Unicode map    
        

    // In the pattern we hope to extract font name 
    // and text strings which are belong to  the font name 
    textPattern = "\\s+/(";
    for (fontIter = m_FontList.begin(); fontIter != m_FontList.end(); fontIter++)
    {
        // Get font name
        fontName = *fontIter;
        // Add escape character "\" befor "+"
        plusPos = fontName.find('+');
        if (plusPos != string::npos)
        {
            fontName = string(fontName.c_str(), plusPos) 
                       + "\\" 
                       + string(fontName, plusPos, fontName.size() - plusPos + 1);
        }
        // Add font name to pattern
        textPattern += fontName + "|"; 
    }

    // Set regex for text context
    textPattern[textPattern.size() - 1] = ')';
    textPattern = textPattern + "\\s+[\\d\\.\\+\\-]+\\s+Tf";
    
    setRegex(&regFontName, textPattern.c_str());
    
    // Set regex for ((?!tar).)+ random string within tar
    setRegex(&regTextString, "\\[?((((\\(((?!T[Jj])(?!>>).)+\\))|(\\<[0-9A-Fa-f]+\\>))\\-?[\\d\\.]*)+)\\]?\\s?T[Jj]"); 
    //cout << textPattern << endl;

    setRegex(&regEndStream, "endstream");
    
    fstream fLog;
    fLog.open("log.txt", ios::out | ios::app);

    /*
     * String variable overlap will store last text buffer which was successfully translated
     * by comparing overlap with new buffer we can skip overlaped texts in PDF
     */ 
    overlap.clear();
    
    // Capture font and its related text content
    while ((numMatched = pcre_exec(regFontName, NULL, searchBuffer, searchLength, 
    //while ((numMatched = pcre_exec(regFontName, NULL, test, tt.size() + 1, 
                                fontOffset, NO_OPTION, fontMatch, MAX_MATCH)) > 0)
    {
        // Capture font name
        pcre_get_substring((const char*)searchBuffer, fontMatch, numMatched, 1, &subStr);
        //pcre_get_substring((const char*)test, fontMatch, numMatched, 1, &subStr);
        fontName = string(subStr);
        //fLog << fontName << endl;
        pcre_free_substring(subStr);
        
        contextSize = -1;

        // Find text context border
        // Find next font name label
        if ((numMatched = pcre_exec(regFontName, NULL, searchBuffer, searchLength, 
                                    fontMatch[1], NO_OPTION, contextMatch, MAX_MATCH)) > 0)
        {
            contextSize = contextMatch[0] - fontMatch[1] + 1;
        }
        // Find endstream label
        if ((numMatched = pcre_exec(regEndStream, NULL, searchBuffer, searchLength,
                                    fontMatch[1], NO_OPTION, contextMatch, MAX_MATCH)) > 0) 
        {
            // If can't find next font name or endstream is beffer 
            if (contextSize < 0 || contextSize > (contextMatch[0] - fontMatch[1] + 1))
            {
                contextSize = contextMatch[0] - fontMatch[1] + 1;
            }
        }
        // Otherwise use total temp end
        if (contextSize < 0)
        {
            contextSize = m_TempLength - fontMatch[1] + 1;
            contextMatch[0] = fontMatch[1];
        }
        // Capture text context
        contextBuffer = searchBuffer + fontMatch[1];
        
        /* Debug */
        //fLog << "********" << endl;
        
        //fLog.write(contextBuffer, contextSize);
        //fLog << endl<< "+++++++" << endl;
        
        // Match text string in captured context
        textOffset = 0;
        //string text = "";
        while ((numMatched = pcre_exec(regTextString, NULL, contextBuffer, contextSize, 
                                       textOffset, NO_OPTION, textMatch, MAX_MATCH)) > 0)
        {
            // Capture text string
            pcre_get_substring((const char*)contextBuffer, textMatch, numMatched, 1, &textBuffer);
            textSize = textMatch[3] - textMatch[2];

            // Output text buffer for debug
            //fLog << "@@" << endl;
            //fLog.write(textBuffer, textSize);
            //fLog << endl << "##" <<endl;
            
            // Decode this string

            if (overlap.size() < 7 || overlap != string(textBuffer, textSize))
            {
                getText(fontName, textBuffer, textSize);
                overlap = string(textBuffer, textSize);
            }
            // else
		  // cout << overlap << endl;
            pcre_free_substring(textBuffer);
            textOffset = textMatch[1];
        }
        
        //pcre_free_substring(contextBuffer);       
        // Match next
        fontOffset = fontMatch[1];
    }

    // Release resoucres
    fLog.close();
    pcre_free(regFontName);
    pcre_free(regTextString);
    return 0;
}

/*
 * Function:     loadExternalCMap
 * Description:  Load CMaps from external file
 * Input:        mapName: external file name
 * Output:       None       
 * Return:       ot: Error occured
 *               0:  Successfully executed
 */
int PDFParser::loadExternalCMap(const char* mapName)
{
    fstream mapFile;                // File stream of external CMap file
    char* mapFileBuffer;            // Buffer of external CMap file
    int objOffset;                  // Offset of captured object
    size_t mapFileLength;           // File length of CMap file
    size_t objLen;                  // Length of captured object
    FontList::iterator fontIter;    // Iterator of font list 
    FontQuote::iterator  quoteIter; // Iterator of Character to Unicode map
    
    // Open external CMap file
    mapFile.open(mapName, ios::in | ios::binary);
    CHECKFILE(mapFile.is_open(), mapName);

    // Get file size
    mapFile.seekg(0, ios::end);
    mapFileLength = mapFile.tellg();
    mapFile.seekg(0, ios::beg);

    // Allocate space for file buffer
    mapFileBuffer = new char[mapFileLength];
    
    mapFile.read(mapFileBuffer, mapFileLength);

    // Read CMaps from file buffer
    getCMap(EX_CMAP, mapFileBuffer, mapFileLength);

    for (quoteIter = m_FontQuote.begin(); quoteIter != m_FontQuote.end(); quoteIter++)
    {
        //cout << quoteIter->first << ":" << quoteIter->second << endl;
        // Find font descriptor object
        findObject(quoteIter->second, objOffset, objLen);
        
        // Check whether this font need external CMap or not
        if (string(m_TempBuffer + objOffset, objLen).find("Identity-H") != string::npos)
        {
            m_MapInfo.insert(MapInfoPair(quoteIter->first, EX_CMAP));
        }
        
    }
    /*
    // For each font build external cmap information
    for (fontIter = m_FontList.begin(); fontIter != m_FontList.end(); fontIter++)
    {
        cout << *fontIter;
        m_MapInfo.insert(MapInfoPair(*fontIter, EX_CMAP));
    }    
    */
    // Release resources
    delete[] mapFileBuffer;
    mapFile.close();

    return 0;
}

/*
 * Function:     mergeCMap
 * Description:  Merged  CMaps for specific font
 * Input:        fontName: font name of CMaps owner
 * Output:       None       
 * Return:       ot: Error occured
 *               0:  Successfully executed
 */
int PDFParser::mergeCMap(string fontName)
{
    CMap mergedMaps[MAX_WIDTH + 1];     // Skip meta at index 0
    size_t codeWidth;                   // Code width
    MapInfo::iterator mapSeqIter;       // Map sequence iterator
    MapsTable::iterator mapIter;        // Map iterator
    MapBody* mapBody;                   // Pointer of map body
    MapBody::iterator codeIter;         // Codes iterator in a CMap
    
    // CMap sequence range of current font
    pair<MapInfo::iterator, MapInfo::iterator> mapSeqRange;      

    /**/
    pair<MergedMap::iterator, MergedMap::iterator> mergedMapRange;
    MergedMap::iterator mergedMapIter;
    

    mapSeqRange = m_MapInfo.equal_range(fontName);
    
    // Merge maps having same code width of currenct font 
    for (mapSeqIter = mapSeqRange.first; mapSeqIter != mapSeqRange.second; mapSeqIter++)
    {
        // Fetch a CMap of current font
        mapIter = m_CMaps.find(mapSeqIter->second);
        codeWidth = (mapIter->second).codeWidth;
        mapBody = &((mapIter->second).mapBody);
        
        // Copy its map body to merged map according its code width
        for (codeIter = mapBody->begin(); codeIter != mapBody->end(); codeIter++)
        {
            // Find conflict: same character code different UCS2 code
            if ( mergedMaps[codeWidth].mapBody.find(codeIter->first) != 
                 mergedMaps[codeWidth].mapBody.end())
            {
                if ((mergedMaps[codeWidth].mapBody.find(codeIter->first))->second != codeIter->second)
                {
                    cout << "***" << endl;
                    cout << (mergedMaps[codeWidth].mapBody.find(codeIter->first))->first
                         << ":"
                         << (mergedMaps[codeWidth].mapBody.find(codeIter->first))->second << endl;
                    cout << codeIter->first << ":" << codeIter->second << endl;
                }
            }
            else
            {
                mergedMaps[codeWidth].mapBody.insert(MapBodyPair(codeIter->first, codeIter->second)); 
            }
        }
    }
    
    // Insert merged maps into MergedMaps container
    for (int i = 1; i <= MAX_WIDTH; i++)
    {
        // Skip empty merged maps
        if ((mergedMaps[i].mapBody).size() != 0)
        {
            mergedMaps[i].codeWidth = i;
            m_MergedMaps.insert(MergedMapPair(fontName, mergedMaps[i]));
        }
    }
    /* 
    mergedMapRange = m_MergedMaps.equal_range(fontName);
    for (mergedMapIter = mergedMapRange.first; mergedMapIter != mergedMapRange.second; mergedMapIter++)
    {
        cout << (mergedMapIter->second).codeWidth << ":" << (mergedMapIter->second).mapBody.size() << endl;
    }
    */
    return 0;
}

/*
 * Function:     getText
 * Description:  Decode textString using specific font
 * Input:        fontName: name of font used in this string
 *               textStr: point of text string buffer for decoding
 *               textLen: length of text string buffer
 * Output:       None       
 * Return:       ot: Error occured
 *               0:  Successfully executed
 */
int PDFParser::getText(string fontName, const char* textStr, size_t textLen)
{
    pcre* regHex;                       // Regex for text written in hexadecimal form
    pcre* regLit;                       // Regex for literal text
    u_short charCode;                   // PDF inner Character code
    u_short ucsCode;                    // UCS-2 code
    int hexMatch[MAX_MATCH];            // Match result of hex part in text string
    int litMatch[MAX_MATCH];            // Match result of literal part in text string
    int hexOffset = 0;                  // Hex offset
    int litOffset = 0;                  // Literal offset
    int numMatched = 0;                 // Matched # of strings
    int codeWidth = 0;                  // Code width of current CMap
    int decodeMethod;                   // Decode directly or using CMap
    int decodeFlag = 0;                 // Decode successful or not
    size_t charSize;                    // Character size of UTF-8 code
    const char* subStr = NULL;          // Temp for extracted sub text string
    string codeString;                  // Encoded code string
    Literal curWord;                    // Current handling word in literal mode
    char escapeChar;                    // Escapte character 
    char utfCode[4];                    // UTF-8 Code for output
    CharString  capString;              // Captured character string
    CharString charString;              // Character string in a sub string
    ExtractResult rsltSet;              // Set of text extract result
    ExtractResult::iterator rsltIter;   // Iterator of result set 
    MergedMap::iterator mergedMapIter;  // Merged map iterator
    MapBody *mapBody;                   // Map body of merged map
    MapBody::iterator codeIter;         // Codes iterator in a CMap

    // Range of merged map of current font
    pair<MergedMap::iterator, MergedMap::iterator> mergedMapRange;
    
    /*
     *  Output variables for debug
     */
    fstream logFile;                // Log file 
    fstream textFile;               // Text file
    
    // Generate text file path of current PDF file
    string  textFilePath;
    textFilePath =  m_CurFileName + ".txt";
    textFile.open(textFilePath.c_str(), ios::out | ios::app);

    // Set pcre regex structure
    setRegex(&regHex, "<([0-9A-Za-z]+)>");
    setRegex(&regLit, "[^\\\\]?\\((((?![^\\\\]\\().)+[^\\\\]?)\\)");
 
    // Match text contained in "< >"
    while ((numMatched = pcre_exec(regHex, NULL, textStr, textLen, 
                                   hexOffset, NO_OPTION, hexMatch, MAX_MATCH)) > 0)
    {
        // Get sub string for extraction 
        pcre_get_substring((const char*)textStr, hexMatch, numMatched, 1, &subStr);
        
        // Insert into result set index by match offset
        capString.type = HEX_CODE;
        capString.content = string(subStr, hexMatch[3] - hexMatch[2]);
        rsltSet.insert(ExtractResultPair(hexMatch[0], capString));
        pcre_free_substring(subStr);
        
        hexOffset = hexMatch[1];
    }
    
    /*
     * log file for test 
     */ 
    fstream llog;
    llog.open("llog", ios::out);

    // Match text contained in "( )"
    while ((numMatched = pcre_exec(regLit, NULL, textStr, textLen, 
                                   litOffset, NO_OPTION, litMatch, MAX_MATCH)) > 0)
    {
        // Get sub string for extraction 
        pcre_get_substring((const char*)textStr, litMatch, numMatched, 1, &subStr);

        capString.type = LIT_CODE;
        // Handle escape char of literal string
        for (int i = 0; i < litMatch[3] - litMatch[2]; )
        {
            // Insert no-escape char directly
            if (subStr[i] != '\\')
            {
                capString.content.insert(capString.content.end(), subStr[i]);
                i++;
            }
            // Translate escape char
            else
            {
              
                switch (subStr[i + 1])
                {
                    case 'a':
                        escapeChar = 0x07;
                        break;
                    case 'b':
                        escapeChar = 0x08;
                        break;
                    case 't':
                        escapeChar = 0x09;
                        break;
                    case 'n':
                        escapeChar = 0x0A;
                        break;                    
                    case 'v':
                        escapeChar = 0x0B;
                        break;
                    case 'f':
                        escapeChar = 0x0C;
                        break;
                    case 'r':
                        escapeChar = 0x0D;
                        break;
                    case '\"':
                        escapeChar = 34;
                        break;                    
                    case '\'':
                        escapeChar = 39;
                        break;
                    case '?':
                        escapeChar = 63;
                        break;
                    case '\\':
                        escapeChar = 92;
                        break;
                    case '0':
                        escapeChar = 0;
                        break;
                    case '2':
                        if (isNumber(subStr[i + 2]) && isNumber(subStr[i+3]))  
                        {
                            escapeChar = '\"';
                            i += 2;
                            break;
                        }
                        else
                        {
                            break;
                        }
                    default:
                        escapeChar = subStr[i + 1]; 
                }
                // Insert this character into string
                capString.content.insert(capString.content.end(), escapeChar);
                i += 2;
            }        
        }
        // Insert into result set
        rsltSet.insert(ExtractResultPair(litMatch[0], capString));
        pcre_free_substring(subStr);
        //llog.write(capString.content.c_str(), capString.content.size());
        //llog<<endl;
        
        // For next match
        capString.content.clear();
        litOffset = litMatch[1];
    }    
    llog.close();

    // Get merged CMaps of current font 
    mergedMapRange = m_MergedMaps.equal_range(fontName);
    // No CMaps belong to this font 
    if (mergedMapRange.first == mergedMapRange.second)
    {
        decodeMethod = NO_CMAP;     // Decode directly
    }
    else
    {
        decodeMethod = USE_CMAP;    // Decode with related CMaps
    }

    // Decode character strings order by position
    for (rsltIter = rsltSet.begin(); rsltIter != rsltSet.end(); rsltIter++)
    {
        // Get character string of second field
        charString = rsltIter->second;
        
        // Check decode method
        if (decodeMethod == NO_CMAP)
        {
            // Output dierectly
            for (size_t i = 0; i < charString.content.size(); i++)
            {
                // Filter extra ASCII having value larger than 0x7F
                if (charString.content[i] > 128 || charString.content[i]  < 0)
                {
                    // Back after removing one illegal character
                    charString.content.erase(i--, 1);
                }
            }
            textFile << charString.content;
        }
        else
        {
            // Poll each merged map of this font
            for (mergedMapIter = mergedMapRange.first; 
                 mergedMapIter != mergedMapRange.second;)
            {
                // Hex mode split text string by code width
                if (charString.type == HEX_CODE)
                {
                    codeWidth = mergedMapIter->second.codeWidth;
                }
                // Literal mode split text string by bytes
                else
                {
                    codeWidth = mergedMapIter->second.codeWidth / 2;
                }
                // Get map body of this CMap
                mapBody = &(mergedMapIter->second.mapBody);
                // cout << charString.content << endl;
                // Try to decode by this CMap
                for (size_t i = 0; i < charString.content.size(); i += codeWidth)
                {
                    // cout << charString.content.size() << " " << i << " "<< codeWidth << endl;
                    // cout << charString.content<<endl;
                    
                    // Capture code by code width
                    codeString = string(charString.content, i , codeWidth);
                    //cout << "111" << endl;
                    
                    // Turn hex value into interger in hex mode
                    if (charString.type == HEX_CODE)
                    {
                        charCode = hex2int(codeString.c_str());
                    }
                    // Turn byte of code width length into interger in literal mode
                    else
                    {
                        memset(curWord.buffer, 0 , 2);
                        // cout << "..." << endl;
                        for (int j = 0; j < codeWidth; j++)
                        {
                            // High bytes at high address
                            curWord.buffer[codeWidth - 1 - j] = charString.content.c_str()[i + j];
                            //printf("%hhx ", charString.content.c_str()[i + j]);
                        }
                        // cout << "\n***" << endl; 
                        charCode = curWord.code;
                        // cout << charCode << endl;
                    }
                    codeIter = mapBody->find(charCode);

                    // Failed decoding try next CMap
                    if (codeIter == mapBody->end())
                    {
                        mergedMapIter++;
                        break;
                    }
                    else
                    {

                        ucsCode = codeIter->second;             // Read UCS-2 code contained in map
                        // Since Non-breaking space doesn't exist in GB
                        // Here We replace this code with normal space
                        if (ucsCode == 160)
                        {
                            ucsCode = 32;
                        }
                        else if (ucsCode >= 0x201E && ucsCode <= 0x2E7F)
                        {
                            ucsCode = 32;
                        }
                        charSize = ucs2utf(utfCode, ucsCode);   // Translate to UTF-8
                        textFile.write(utfCode, charSize);      // Write to file
                        // cout << charCode << " " << string(utfCode, charSize) << endl;
                        memset(utfCode, NULL, 4);               // Clear buffer
                        decodeFlag = 1;                         // Decode successful
                    }
                }
                if (decodeFlag > 0)
                    break;
            }
            // Failed decoding with CMaps 
            if (!decodeFlag)
            {
                textFile << charString.content;
                /*
                if (charString.content == "en-US")
                {
                    cout << "wang" << endl;
                    
                    cout <<string(textStr, textLen)<<endl;
                    cout << "yun" << endl;
                }
                */
                    
            }
            else
            {
                decodeFlag = 0;
            }
        }
    }

    // Release resources
    pcre_free(regHex);
    pcre_free(regLit);
    textFile.close(); 
    return 0;
}
