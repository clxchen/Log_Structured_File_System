//
//  CheckPointRegion.hpp
//  
//
//  Created by Matthew Myers on 5/3/16.
//
//

#include <math.h>
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <fstream>
#include <string>
#define BLOCK_SIZE 1024
#define SEGMENT_SIZE 1024*BLOCK_SIZE
#define MAX_BLOCKS_PER_FILE 128

using namespace std ;
class CheckPointRegion
{
public:

	int isMapLocUsed[40] ;


	CheckPointRegion() ;
	void updateMapPart(int mapPart, int blockNum) ;
	void UpdateCPR(char *);
	void addImap(int blockNum);
	void saveImap(int * saveImapArray);
	void changeLive(int segment);
	bool isUsed(int segment);
	void updateFile(char * name) ;
	int * arrayOfImapLocations() ;
	int getFreeSeg() ;

	int liveSeg ;
    	int liveMaps ;
    

    char* bytes = new char[192] ;
    //161 - 192 is used 'bits'
    
    
};
