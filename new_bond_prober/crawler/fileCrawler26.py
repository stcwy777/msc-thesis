# -*- coding: gb2312 -*-

# Extract announcements' URL from specific web page and download these files
"""
@version: 1.1
@author: Wang Yun 
@see: ICRC Docs
@history:
 1) Edit codes to meet new structure of target web site cninfo
 2) Only check first two pages
"""

import urllib
import re
import os

STOP = 0;
FINISH = 1;
TOTAL_PAGES = 2;
PATH = "/home/datagroup/wangyun/bond_prober/crawler/"
"""
def pathSwitch()
    result = {
        'a': lambda x: x * 5,
        'b': lambda x: x + 7,
        'c': lambda x: x - 2
    }[value](x)
    return result
"""
PATH_DICT= {"上市公告":"list", "发行公告":"issue", "票面利率公告":"rates"};

class URLCrawler:
    """
    This class will extract URL of required announcements from specific web page
    and download these annoucements to local client.
    """

    def __init__(self):
        """
        Class constructor
        """
        self.cur_update = "";     # File ID of current updated file
        self.last_update = "";    # File ID of last updated file

    def getURL(self, source_url):
        """
        Extract URLs of required announcements
        @source_url: source web page from where crawler extracts URLs
        """
        
        # Connect server and request web page
        try:
            source_page = urllib.urlopen(source_url);        
        except urllib.URLError(error):
            print("Failed to connect the server.");
            print("Reason:", error.code)
        except urllib.HTTPError(error):
            print("Failed to open this URL.");
            print("Code:", error.code);        
        
        
        page_str = source_page.read();                   # Strore source code of web page in buffer      
        #page_str = page_buffer.decode("gb2312");        # Decode bytes variable into string using 'gb2312'
        #print page_buffer;
        
        # Test code
        """
        page_restore = open("restore.html","wb");
        page_restore.write(page_buffer);
        """

        """
        Build regular expression to find announcements
        with following features:
          上市公告
          发行公告
          票面利率公告
        """
        re_pattern = """
                     (finalpage/([0-9]{4}-[0-9]{2}-[0-9]{2})(\S*)PDF)  # PDF URL of annoucements
                     '\starget='_blank'>                               # Middle html label
                     ([^<]+((上市|发行|票面利率)公告)[^<]*)            # Annoucements' name in Chinese
                     """;
        # Use this pattern to match URLs in the page source
        re_match = re.compile(re_pattern, re.X);
        re_result = re_match.findall(page_str);
        # Test output
        # print re_result[0][3];
        return re_result;

    def getFiles(self, url_list, pos, new_list):
        """
        Download files and save to local
        @url_list: info extracted by crawler
        @pos: sz or sh
        """
        # Read record in log file
        file_log = open(PATH + "update_" + pos + ".log", "r");
        update_record = file_log.read();
        file_log.close();
        
        #check = open("check", "w");
        for i in range(len(url_list)):
            # This file has been updated stop downloading
            if update_record == url_list[i][0]:
                print("No more new files, stop updating");
                return STOP;
            print pos + "\n";
            
            if url_list[i][3].find("转") != - 1 or  url_list[i][3].find("暂停") != - 1:
                continue;
            for j in range(4):
                print (url_list[i][j]);

            try:
                # Generate complete URL and local file path
                comp_url = "http://www.cninfo.com.cn/" + url_list[i][0];
                 
                #local_path = "PDF_backup\\" + PATH_DICT[url_list[i][4]] + "\\" + url_list[i][1] + '_' + url_list[i][3] +  ".pdf";
                local_path = PATH + "../pdf_backup/" + PATH_DICT[url_list[i][4]] + "/" + url_list[i][1] + '_' + url_list[i][2][1:-1] +  ".pdf";
                # Record 上市公告
                if url_list[i][4] == "上市公告":
                    new_list.append(local_path);

                #print ("update file" + local_path + "\n");
                # Request and download files
                print comp_url;
                print local_path;
                urllib.urlretrieve(comp_url, local_path);
            except urllib.ContentTooShortError:
                print("Faild to download file.");
            
            # Record lastest file info
            if i == 0 and self.cur_update == "":
                self.cur_update = url_list[i][0];
        return FINISH;
    
    def updateLog(self, pos):
        """
        If crawler executed updating, it will rewrite the log file
        """

        if self.cur_update != "":
            # Read record in log file
            file_log = open(PATH + "update_" + pos + ".log", "w");
            file_log.write(self.cur_update);
            file_log.close();
            

        

# Main code
if __name__ == '__main__':
    os.system("date"); 
    
    crawler = URLCrawler();
    # Source page from cninfo
    # Here we use new url and sz sh have different urls --version 1.1
    # Begin with sh
    source_url = "http://www.cninfo.com.cn/disclosure/bond/shbdlatest.html";
    pos = "sh";
    new_list = [];
    file_new = open(PATH + "new_list", "w");

    for  i in range(2):        
        # TOTAL_PAGES will checked
        for j in range(TOTAL_PAGES):
            url_list = crawler.getURL(source_url);
                
            # Generate next source URL
            if (j == 0):
                source_url = source_url.replace(".html", "2.html");         # e.g. "szbdlatest.html" to "szbdlatest2.html"
            """
            # Not used since # of checkede page < 2
            elif (j < 4):
                source_url = source_url.replace(str(j + 1), str(j + 2));   # e.g. "szbdlatest2.html" to "szbdlatest3.html"
            """

            # Check url_list is avaiable
            if (len(url_list) == 0):
                print ("No matched file on page" + str(j) + " of " + pos);
                continue;
            else:
                # Execute download
                state = crawler.getFiles(url_list, pos, new_list);
                if (state == STOP):
                    break;
        # end for j
        crawler.updateLog(pos);    # Update log file

        # Next step handle sh
        source_url = "http://www.cninfo.com.cn/disclosure/bond/szbdlatest.html";
        pos = "sz";
    # end for i
    for path in new_list:
        file_new.write(path + "\n");
    file_new.close();
    os.system(PATH + "../pdf_miner/PDFMiner");
