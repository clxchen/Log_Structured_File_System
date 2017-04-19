//
//  Segment.hpp
//  
//
//  Created by Matthew Myers on 5/5/16.
//
//


#include <stdio.h>
#include "CheckPointRegion.h"
#include "SegmentSummary.h"

using namespace std ;

struct block_struct
{
    char byte[1024];
};

struct inode
{
    int filesize;
    char filename[256];
    int datablocks[MAX_BLOCKS_PER_FILE];
};

struct imap
{
    int inodeAndBlockNums[256];
    
};


class Segment
{
public:
    
    Segment(int) ;
    // Ints are the block index, blockNum 0-1023 in a segment
    void addImap(imap) ;
    void saveBlock(block_struct) ;
    void saveInode(inode) ;
    void saveImap(int * saveImapArray) ;
    
    void saveToDrive() ;
    
    int freeBlocks;
    int blockIndex;
    string segName;
    char bytes[1024*1024] ;

    
    
    SegmentSummary SegSum;
    int imapBlock;
    int segNumber;

    //161 - 192 is used 'bits'
    
    
};
