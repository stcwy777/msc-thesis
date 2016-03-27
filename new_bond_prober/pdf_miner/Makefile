#This Makefile is used for PDF2TextConv 

CC = g++ 
CFLAGS = -g -Wall -DOS_LINUX
INCS = -I. 
LIBS = -L. -L/usr/local/lib -lz -lpcre -L./word_segment -lICTCLAS50 -liconv -L/usr/lib/mysql -lmysqlclient
OBJS = PDFParser.o WordAnalyzer.o InfoMiner.o MySQL.o main.o codetransformer.o

PDFMiner:$(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)
.cpp.o:	
	$(CC) $(CFLAGS) $(INCS) -c $< 

clean:
	rm -f  $(OBJS) 
	rm -f PDFMiner
#EOF
