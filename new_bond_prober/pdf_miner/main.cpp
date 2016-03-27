/***************************************************************
 * Copyright (c) 2010.3  Wang Yun
 * @ Harbin Institute of Technology ShenZhen Graduate School
 * 
 * PDF2Text Convertion
 *
 * Main process to handle, still testing
 *
 **************************************************************/

#include "common.h"
#include "PDFParser.h"
#include "WordAnalyzer.h"
#include "codetransformer.h"
#include "InfoMiner.h"

int main(int argc, char *argv[])
{
    string pdfPath;
    string textPath;
    string segPath;
    string transBuffer;
    string segResult;
        
    ifstream pdfList;
    ifstream textFile;
    ofstream segFile;
    ifstream ruleFile;
    ofstream errorLog;

    vector<string> fileDate;
    char* textBuffer;
    size_t textFileLength;
    PDFParser pdfExtractor;    
    WordAnalyzer wordAnalyzer;
    CCodeTransformer cm;
    InfoMiner im;
    //cout << ICTCLAS_ImportUserDict("BondDict.txt");
    /*
     * Rule File
     */
    //ruleFile.open("simple.rule", ios::in);
    ruleFile.open("/home/datagroup/wangyun/bond_prober/pdf_miner/simple.rule", ios::in);
    size_t itemNum;
    size_t ruleNum;
    size_t ruleID;
    string pattern;
    

    ruleFile >> itemNum;
    ruleFile >> ruleNum;
    
    Rule simpRules[itemNum];

    for (size_t i = 0; i < ruleNum; i++)
    {
        ruleFile >> ruleID;
        ruleFile >> simpRules[ruleID].tarNum;
        ruleFile >> simpRules[ruleID].defValue;
        ruleFile >> pattern;
        
        //cout << ruleID << " " << pattern << endl;
        simpRules[ruleID].pattern.push_back(pattern);
    }
    
    //pdfList.open("new_list", ios::in);
    pdfList.open("/home/datagroup/wangyun/bond_prober/crawler/new_list", ios::in);
    //pdfList.open("market/list.txt", ios::in);
    //pdfList.open("file_list.txt", ios::in);
    //errorLog.open("error_log", ios::out | ios::app); 
    errorLog.open("/home/datagroup/wangyun/bond_prober/pdf_miner/error_log", ios::out | ios::app); 
    
    while (!pdfList.eof())
    {
        pdfList >> pdfPath;
        cout << pdfPath;
        if (pdfPath.length() > 0)
        //filePath = "PDF_backup/2010-03-24_杉杉股份：2010年公司债券发行公告.pdf";
        //for (int i = 0 ; i < 2; i++)
        {            //cout << filePath << endl;
            //pdfPath = "PDF_backup/" +  pdfPath;
            //pdfPath = "market/" +  pdfPath;
            //pdfPath = "un/" +  pdfPath;
            //pdfPath = "PDF_backup/2010-01-29_09铁岭债：2009年公司债券上市公告书.pdf";
            //pdfPath = "PDF_backup/2010-03-09_09沈国资：2009年公司债券上市公告书.pdf";
            //pdfPath = "PDF_backup/2010-01-15_09盘锦债：2009年公司债券上市公告书.pdf";
            //pdfPath = "PDF_backup/2010-01-26_10北汽投：2010年公司债券发行公告.pdf";
            //pdfPath = "PDF_backup/2010-03-25_10长高新：债券上市公告.pdf";
            //pdfPath = "market/2010-05-07_10辽源债：上市公告书.pdf";
            //pdfPath = "market/2010-04-01_10鲁商债：上市公告书.pdf";
            //pdfPath = "market/2010-06-11_中国银行：可转换公司债券上市公告书.pdf";
            //filePath = "market/2010-09-13_10通辽债：上市公告书.pdf";
            //filePath = "market/2010-06-29_10通产控：上市公告书.pdf";
            //filePath = "market/2010-11-02_10中铁G3：上市公告书.pdf";
            //filePath = "market/2010-06-11_中国银行：可转换公司债券上市公告书.pdf";
            //filePath = "market/2010-11-08_10中关村：上市公告书.pdf";
            //filePath = "market/2010-05-28_宜华木业：关于“09宜华债”上市公告书.pdf";
            //filePath = "market/2010-06-11_中国银行：可转换公司债券上市公告书.pdf";
            //filePath = "market/2010-06-29_新黄浦：关于“09新黄浦”上市公告书.pdf";
            //filePath = "market/2010-04-01_10阜阳债：上市公告书.pdf";
            //filePath = "market/2010-09-13_10通辽债：上市公告书.pdf";
            //filePath = "market/2010-11-11_10冶色债：上市公告书.pdf";
            //filePath = "market/2010-04-13_杉杉股份：公司债券上市公告书.pdf";
            //filePath = "market/2010-07-02_10长城投：上市公告书.pdf";
            //filePath = "PDF_backup/2010-01-27_中国中铁：2010年中国中铁股份有限公司公司债券（第一期）票面利率公告.pdf";
            //filePath = "PDF_backup/2010-03-24_杉杉股份：2010年公司债券发行公告.pdf";
            //filePath = "PDF_backup/2010-01-14_09招金债：2009年公司债券上市公告书.pdf";
            //pdfPath = "PDF_backup/2010-01-16_紫江企业：2009年公司债券上市公告书.pdf";
            //filePath = "PDF_backup/2010-02-03_10首机01：公开发行2010年（第一期）公司债券票面利率公告.pdf";
            //filePath = "PDF_backup/2010-01-27_大亚科技：2010年公司债券发行公告.pdf";
            //filePath = "PDF_backup/2010-02-26_湖北宜化：公司债券上市公告书.pdf";
            //filePath = "PDF_backup/2010-01-26_金丰投资：关于“09金丰债”上市公告书.pdf";
            //filePath = "PDF_backup/2010-03-24_杉杉股份：2010年公司债券发行公告.pdf";
            
	    
	    //filePath = "PDF_backup/2010-03-24_10北汽投：2010年公司债券上市公告书.pdf";
            //filePath = "PDF_backup/2010-01-29_大亚科技：2010年公司债券票面利率公告.pdf";
            //pdfPath = "PDF_backup/2010-01-14_09招金债：2009年公司债券上市公告书.pdf";
            //filePath = "PDF_backup/2010-03-09_大亚科技：公司债券上市公告书.pdf";
            //filePath = "PDF_backup/2010-03-25_10长高新：债券上市公告.pdf";
            fileDate = split(pdfPath, '/', fileDate);
            string dataInfo = fileDate[9];
	    fileDate = split(dataInfo, '_', fileDate);
	    //pdfPath = "new/" + pdfPath; 
            pdfExtractor.read(pdfPath.c_str()); 
            cout << "Extracting File:" << pdfPath << endl;
            
            // Error extracting pdf file
            if (pdfExtractor.execExtract() < 0)
            {
                errorLog << "Date:" << fileDate[0] << endl
                         << "FileName:" << pdfPath << endl
                         << "Error: Unextractable PDF Document" << endl; 
            }
            
            // Generate text file path
            textPath = string(pdfPath.c_str(), pdfPath.size() - strlen(".pdf")) + ".txt";
            textFile.open(textPath.c_str(), ios::in | ios::binary);
            CHECKFILE(textFile.is_open(), textPath.c_str());

            // Get text file length and allocate space
            textFile.seekg(0, ios::end);
            textFileLength = textFile.tellg();
            textFile.seekg(0, ios::beg);
            textBuffer = new char [textFileLength];

            // DEBUG
            //fstream test;
            //test.open("testsg.txt", ios::out);

            // Read text data
            textFile.read(textBuffer, textFileLength);
            
            // Handle English number
            transBuffer = wordAnalyzer.handleEngNumber(textBuffer, textFileLength);
            // Segment text
            // segResult = wordAnalyzer.segmentParagraph(transBuffer); 
            
            // DEBUG
            //test << cm.utf2gb(transBuffer);
            //test.close();

            // Save segmentation result to .seg file
            // segPath =string(pdfPath.c_str(), pdfPath.size() - strlen(".pdf")) + ".seg";
            // segFile.open(segPath.c_str(), ios::out | ios::trunc);
            // CHECKFILE(segFile.is_open(), segPath.c_str());
            // segFile << segResult;
            // segFile.close();

            string word;
            //cout << transBuffer.size() << " " << textFileLength << endl;
            // --- + textBuffer: stored text content
            // --- + segResult: word content
            //im.protype(textBuffer, textFileLength);
            //cout << transBuffer << endl;
            cout << fileDate[0] << endl;
            if (im.protype(transBuffer.c_str(), transBuffer.size(), itemNum, simpRules, fileDate[0]) < 0)
            {
                errorLog << "Date:" << fileDate[0] << endl
                         << "FileName:" << pdfPath << endl
                         << "Error: Core Information Missed" << endl;            
            }
            
	    //im.segmantic(transBuffer.c_str(), transBuffer.size()); 
            // Release resources for next extractt
            textFile.close(); 
            delete[] textBuffer;
            
            pdfPath = "";
            fileDate.clear();
            //break;
        }
    }
     
    // display result
    //printf("Hello World!");
    errorLog << "----------------------------------------------------------" << endl; 
    pdfList.close();
    errorLog.close();
    //textFile.close();
    return 0;
}
