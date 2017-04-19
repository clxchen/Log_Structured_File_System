#include <iostream>
#include <vector>
#include <fstream>
#include <deque>
#include "Segment.h"
#include <string>
#define BLOCK_SIZE 1024
#define SEGMENT_SIZE 1024*BLOCK_SIZE
#define MAX_BLOCKS_PER_FILE 128
#include <sstream> 
using namespace std;


class Directory
{
    public:
    Directory();
    void DirectoryImport(string, string);
    void remove(string filename) ;
	void list() ;
    void SetBlocksZero();
    void SetInodeZero();
    void SetImapZero();
    void SaveBlockIndexToInode(int, int);
    void UpdateFNM(int, string);
	int findFreeInode();
	void updateImap(int,int);
    int ReturnBlocksInFile(string);
    void Overwrite(string, int, int, char);
    void cat(string);
    int ReturnBytesInFile(string filename);
    void Display(string LFSfilename, int howmany, int start);
    void Clean( int amountToClean );
    
    imap saveThisImap;
    inode saveThisInode;
	void exitD() ;
    
    block_struct workingBlock;
    vector<Segment> Drive;
    
    char inMemorySegment[SEGMENT_SIZE];
    int amountRead;
    int driveSegIndex;
	int imapNumber;

	int imapFull[(256*40)] ;
	int imapPart ;

	CheckPointRegion CPR ;
		
    

	string segPath ;

    //added by marty
	int freeInodes[10000] ;
	
	//end added
    imap imapArray[40];
    char readIn[SEGMENT_SIZE];

};
