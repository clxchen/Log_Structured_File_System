//
//  Segment.cpp
//  
//
//  Created by Matthew Myers on 5/5/16.
//
//

#include "Segment.h"

Segment::Segment(int i)
{
    blockIndex = 8;
    //cout << "in Seg construct " << endl;
  char c;
    freeBlocks = 1016;
    string os;
    os = "DRIVE/SEGMENT";
    os.append(to_string( i ));
    
    segName = "DRIVE/SEGMENT";
    segName.append(to_string( i ));
    
    
    ifstream fileCheck;
    fileCheck.open(os);
    if (fileCheck.is_open())
    {
        for (int i = 0; i < 1024*1024; i++)
        {
            c = fileCheck.get();
            bytes[i] = c;
        }
        /* MUST UPDATE IMAPS WITH CPR AND ETC */
    }
    else
    {
        cout << "ERROR COULD NOT OPEN FILE\n\n\n";
        //exit();
    }
    memcpy(SegSum.locArray, bytes, 1024*4);
    /*if ( i == 0 )
    {
    for (int j = 0; j < 1024*4; j ++)
    {
        cout << "SEGSUM ON LOAD I: " << j << " " << SegSum.locArray[j] << "\t";
    }
    }*/
    segNumber = i;
    segName = segName;
    //cout << "out Seg construct " << endl;
    
}

void Segment::addImap( imap passImap)
{
    this->freeBlocks--;
	memcpy(bytes + (1024*blockIndex), &passImap,sizeof(imap));
	blockIndex++;
    
    
}

void Segment::saveImap(int* saveImapArray)
{
	this->freeBlocks--;
	memcpy(bytes + 1024*blockIndex, saveImapArray,1024);
	
	
	blockIndex++;

}
void Segment::saveBlock(block_struct saveThisBlock)
{
    //STORES DATA BLOCKS CORRECTLY - MATT 5/10 3:21 PM
    //Insert Block into Bytes + blockIndex * 1024 of the BytesArray
    this->freeBlocks--;

    memcpy(bytes + 1024*blockIndex, &saveThisBlock, 1024);

    blockIndex++;
}

void Segment::saveInode(inode saveThisInode)
{
    this->freeBlocks--;
    memcpy(bytes + 1024*blockIndex, &saveThisInode, sizeof(inode));
    blockIndex++;
    //Insert Inode into Bytes + blockIndex * 1024 of the BytesArray
    //Have the Inode be listed into the Imap array already
    
    
    
}

void Segment::saveToDrive()
{
    ofstream out;
    out.open(segName);
    int test;
    memcpy(this->bytes, SegSum.locArray, 4*1024);
    //out << "SEGMENT NAME: " << segName << endl;
     for (int i = 0 ; i < 1024*1024; i++)
     {
        out << this->bytes[i];

     }
    out.close();
    
}


