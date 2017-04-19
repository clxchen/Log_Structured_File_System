
#include <stdio.h>
#include <math.h>
#include <iostream>
#include <string.h>
#include <fstream>
#include <string>

using namespace std ;


class SegmentSummary
{
public:

char charArray[8*1024] ;
int locArray[1024] ;

SegmentSummary() ;

void addInodeSS(int nodeNum,int loc) ;

void addImapSS(int loc) ;

void addDataBlockSS(int nodeNum, int loc) ;

char * writeToBlock() ;

};
