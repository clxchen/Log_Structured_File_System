#include "Directory.h"

#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <fstream>
#include <vector>
#include <string>

#define BLOCK_SIZE 1024
#define SEGMENT_SIZE 1024*BLOCK_SIZE
#define MAX_BLOCKS_PER_FILE 128

using namespace std;


//First 8 blocks are the segment summary, skip those

/*=====Constructor=====*/
Directory::Directory()
{
	for(int i=0;i<10000;i++)
		freeInodes[i] = 0;

//	Update CPR with contents of File//
	ifstream checkFile("DRIVE/CHECKPOINT") ;
	char tempCheck[192] ;
	checkFile.read(tempCheck,192) ;
	CPR.UpdateCPR(tempCheck) ;

//Use CPR to update other stuff
	driveSegIndex = CPR.getFreeSeg() ;
	CPR.changeLive(driveSegIndex) ;
	SetImapZero();
	int * imapPieces;
	imapPieces = CPR.arrayOfImapLocations();

	for(int i = 0 ; i < 32 ; i++)
    {
        Segment seg(i);
		Drive.push_back(seg);
	}	

	for ( int i = 0; i < 40; i++)
	{
		if ( imapPieces[i] == 808464432)
		{
			break;
		}
		else 
		{	

			int tempSegIndex = imapPieces[i] / 1024;

			memcpy(&saveThisImap, Drive[tempSegIndex].bytes + (imapPieces[i] % 1024)*1024, sizeof(imap));
            
			memcpy(imapFull + (i * 1024), &saveThisImap,1024);
		}
	}

	for(int i = 0 ; i < 10000 ; i++)
    {
		if(imapFull[i] == 0)
        {
			freeInodes[i] = 0 ;
		}
		else{
			freeInodes[i] = 1 ;
		}

	}	
	int testCheck;
	memcpy(&testCheck, CPR.bytes, 4);

}



int Directory::findFreeInode()
{
	for(int i=0;i<10000;i++)	
	{
		if(freeInodes[i] == 0)
		{
			freeInodes[i] = 1;
			return i;
					
		}
	}
    return -1;
		
}

void Directory::list()
{
	ifstream infile("DRIVE/FILENAMEMAP") ;
	string compFile ;
    	inode ourInode;
	int nodeNum ;
	int toRemove ;
	int offset ;
	string colon ;
	int compare;
	int segIndexList = 0 ;
    
    
	while(infile >> compFile >> colon >> nodeNum)
    {
        segIndexList = imapFull[nodeNum] / 1024;


	offset = (imapFull[nodeNum] % 1024 ) * 1024;
        inode ourInode;
        for (int i = 0; i < 256; i++)
        {
            ourInode.filename[i]='0';
        }
    
        
        ourInode.filesize=0;
				    
        for (int i = 0; i < MAX_BLOCKS_PER_FILE; i++)
        {
            ourInode.datablocks[i] = 0;
        }
        memcpy(&ourInode, Drive[segIndexList].bytes + offset, sizeof(inode));
        cout << "FILE NAME: " << ourInode.filename << " FILE SIZE: " << ourInode.filesize << endl;
	
    }
}
	


void Directory::remove(string LFSfilename){

    /*  Read in the LFSFile name                                                            */
    /*  read through FILENAMEMAP                                                            */
    /*      if LFSname = name in FILENAMEMAP                                                */
    /*          save node num                                                               */
    /*          remove that line from the file                                              */
    /*          search through the imaps and remove the references to the inodes and imap   */
     
     
     
     
	ifstream infile("DRIVE/FILENAMEMAP") ;
	string compFile ;

	stringstream reWrite ;

	int nodeNum ;
	int toRemove ;

	string colon ;
    bool inFile = false;
	while(infile >> compFile >> colon >> nodeNum){
		if(compFile == LFSfilename)
        {
			toRemove = nodeNum ;
            inFile = true;
		}
        else
        {
			reWrite << compFile << " : " << nodeNum << '\n' ;
		}
	}
    if (!inFile)
    {
        cout << "ERROR: FILE NOT IN THE CACHE\n";
        exit(0);
    }
	ofstream outfile("DRIVE/FILENAMEMAP") ;
	outfile << reWrite.str();
    
	freeInodes[toRemove] = 0 ;
	imapFull[toRemove] = 0;
	int saveImapArray[256];
	int start ;
	start = toRemove - (toRemove % 256);
	for (int i = 0; i < 256; i ++)
	{
		saveImapArray[i] = imapFull[start + i] ;
		}
    Drive[driveSegIndex].SegSum.addImapSS(Drive[driveSegIndex].blockIndex);
	Drive[driveSegIndex].saveImap(saveImapArray);

	CPR.updateMapPart(toRemove/256, driveSegIndex*1024 + Drive[driveSegIndex].blockIndex -1 );


    infile.close() ;
}


/*                               Read File,   Save File Name                                */
void Directory::DirectoryImport(string filename, string LFSfilename)
{
    
    /* Take in the file name to save and what to save as                    */
    /* Find size of the file we are reading in,                             */
    /* check current Segment                                                */
    /*      if enough blocks, put it in, with inode                         */
    /*              Inode holds file name, data blocks, file size           */
    /*          else, start a new segment (up to 31 for lab 7)              */
    /*      If the segment has no Imap, make one                            */
    /*          else, update the current one                                */
    /* Update the CheckPointRegion with CheckPointRegion::addImap(blockNum) */
    
    
    
    /* STILL NEEDED:                                                            */
    /*      Check If theres an Imap                                             */
    /*      Update CPR / FILENAMEMAP                                            */
    /*      Check to make sure correct storage                                  */
	imapNumber = CPR.liveMaps;
    SetBlocksZero();
    SetInodeZero();
    
    

    int size = this->ReturnBlocksInFile(filename);
    int bytesInFile = this->ReturnBytesInFile(filename);
    

    if ( size > Drive[driveSegIndex].freeBlocks)
    {
        /*  SAVE THE IMAP TO THE PREVIOUS SEGMENT BEFORE HITTING THE NEXT SEGMENT */
 
        SetInodeZero();
        SetBlocksZero();
        Drive[driveSegIndex].saveToDrive();
        
        
        
        
        driveSegIndex = CPR.getFreeSeg();
        Segment newSeg(driveSegIndex);
        Drive.erase(Drive.begin() + driveSegIndex);
        Drive.insert(Drive.begin()+driveSegIndex, newSeg);
	    CPR.changeLive(driveSegIndex) ;
        //start a new segment
    }
    saveThisInode.filesize = bytesInFile;
    memcpy(saveThisInode.filename, LFSfilename.c_str(), sizeof(LFSfilename.c_str()));


    ifstream inFile;
    inFile.open(filename);
    int byteIndex = 0;
    char c;
    


    int imapIndex = findFreeInode();
    if (imapIndex == -1)
    {
        cout << "MAX INODES REACHED, CANNOT IMPORT\n";
        return;
    }
    
    while (!inFile.eof())
    {
        c = inFile.get();
        workingBlock.byte[byteIndex] = c;
        byteIndex++;
        if (byteIndex == 1024)
        {
            
            Drive[driveSegIndex].SegSum.addDataBlockSS(imapIndex, Drive[driveSegIndex].blockIndex);
            Drive[driveSegIndex].saveBlock(workingBlock);
            SaveBlockIndexToInode(driveSegIndex, Drive[driveSegIndex].blockIndex-1);
            SetBlocksZero();
            byteIndex = 0;
            
        }
        
    }

    if (byteIndex%1024 != 0)
    {
        Drive[driveSegIndex].SegSum.addDataBlockSS(imapIndex, Drive[driveSegIndex].blockIndex);
        Drive[driveSegIndex].saveBlock( workingBlock);
        SaveBlockIndexToInode(driveSegIndex, Drive[driveSegIndex].blockIndex-1);
        byteIndex = 0;
    }
	
        Drive[driveSegIndex].SegSum.addInodeSS(imapIndex, Drive[driveSegIndex].blockIndex);
        Drive[driveSegIndex].saveInode(saveThisInode);

	
	   UpdateFNM(imapIndex, LFSfilename);
	

	imapFull[imapIndex] = driveSegIndex*1024 + Drive[driveSegIndex].blockIndex-1;

	int saveImapArray[256];
	int start ;
	start = imapIndex - (imapIndex % 256);

	for (int i = 0; i < 256; i ++)
	{
		saveImapArray[i] = imapFull[start + i] ;
		}
    Drive[driveSegIndex].SegSum.addImapSS(Drive[driveSegIndex].blockIndex);
	Drive[driveSegIndex].saveImap(saveImapArray);
	
	CPR.updateMapPart(imapIndex/256, driveSegIndex*1024 + Drive[driveSegIndex].blockIndex - 1);
    
    


}

void Directory::SetBlocksZero()
{
    for (int i = 0; i < 1024; i++)
    {
        workingBlock.byte[i] = '0';
    }

}

int Directory::ReturnBlocksInFile(string filename)
{
    FILE * pFile;
    int size;
    int length;
    pFile = fopen (filename.c_str(),"rb");
    if (pFile==NULL)
        perror ("Error opening file");
    else
    {
        fseek (pFile, 0, SEEK_END);
        size=ftell (pFile);
        fclose (pFile);
    }
    length = size - (size % 1024);
    length = length / 1024;
    if (size % 1024 != 0)
    {
        length++;
    }

    return length;
}

int Directory::ReturnBytesInFile(string filename)
{
    FILE * pFile;
    int size;
    int length;
    pFile = fopen (filename.c_str(),"rb");
    if (pFile==NULL)
        perror ("Error opening file");
    else
    {
        fseek (pFile, 0, SEEK_END);
        size=ftell (pFile);
        fclose (pFile);
    }
    
    return size;
}

void Directory::SetInodeZero()
{
    saveThisInode.filesize = 0;
    for (int i = 0; i < 256; i++)
    {
        saveThisInode.filename[i] = '0';
    }
    for (int i = 0; i < 128; i++)
    {
        saveThisInode.datablocks[i] = 0;
    }
}
void Directory::SaveBlockIndexToInode(int workingSegIndex, int blockIndex)
{
	int temp;
	temp = blockIndex + (workingSegIndex * 1024);
    for (int i = 0; i < 128; i++)
    {
        if ( saveThisInode.datablocks[i] == 0)
        {
            saveThisInode.datablocks[i] = temp;
            break;
        }
    }
}

/*                    SAVE THE INODE NUM AND BLOCK INDEX IN FILENAMEMAP                 */
/*         ADDING THE IMAP TO THE SEGMENT SHOULD ADD THE IMAP TO THE CPR                 */
void Directory::UpdateFNM(int nodeNum, string LFSfilename)
{
    //saveThisImap;
    string path = "DRIVE/FILENAMEMAP";
    ofstream out;
    out.open(path, ios_base::app);
    out <<  LFSfilename << " : " << nodeNum << endl;
    
}
void Directory::SetImapZero()
{
    for (int i = 0; i < 256; i++)
    {
        saveThisImap.inodeAndBlockNums[i] = -1;
    }
}

//void Directory::clean(){

	

//}


void Directory::exitD(){

    Drive[driveSegIndex].saveToDrive();
	CPR.updateFile("DRIVE/CHECKPOINT") ;
    cout << "\n------EXITING PROGRAM------" << endl ;

	
	exit(0) ;
	
}


void Directory::Overwrite(string LFSfilename, int howmany, int start, char c)
{ 
    ifstream infile("DRIVE/FILENAMEMAP");
    string compFile ;
    inode ourInode;
    imap ourImap;
    block_struct dataBlock;
    string colon;
    int nodeNum = 0;
    int offset;
    int numBlocks = 0;
    int bytesInFile = 0;
    int InodeNumber = 0;
    int oldFileSize = 0;
    int segIndexList = 0 ;
    
    SetBlocksZero();
    
    while(infile >> compFile >> colon >> nodeNum)
    {
        if(compFile == LFSfilename)
        {
            InodeNumber = nodeNum;
            break;
        }
    }
    int charToWrite = howmany ;
    
    segIndexList = imapFull[InodeNumber] / 1024;
    offset = (imapFull[InodeNumber] % 1024 ) * 1024;
    
    memcpy(&ourInode, Drive[segIndexList].bytes + offset, sizeof(inode));
    
    
    if (start+howmany > ourInode.filesize)
    {
        //cout << "in if" << endl ;
        SetInodeZero();
        for(int i = 0 ; i < 256 ; i++){
            saveThisInode.filename[i] = ourInode.filename[i] ;
        }
        int oldFileSize = ourInode.filesize ;
        int newFileSize = start + howmany ;
        saveThisInode.filesize = newFileSize ;
        //cout << "newFileSize: " << newFileSize << endl ;
        char newFile[newFileSize] ;
        int k ;
        
        for(int i = 0 ; i < ( oldFileSize /1024)+1  ; i ++){
            
            int seg = ourInode.datablocks[i] / 1024 ;
            int offsetG = (ourInode.datablocks[i] % 1024) * 1024 ;
            //cout << "SEg: " << seg << " Offset: " << offsetG << endl ;
            for(int j = 0 ; j < 1024 ; j++){
                if( j > newFileSize){ break ; }
                //cout << "in for" << " " << j << endl ;
                //cout << Drive[seg].bytes[offsetG + j] ;
                k = i *1024 ;
                newFile[k + j] = Drive[seg].bytes[offsetG + j] ;
            }
            
        }
        
        for(int i = start; i < start + howmany ; i++){
            newFile[i] = c ;
        }
        /*
         for(int i = 0 ; i < newFileSize ; i++){
         cout << newFile[i]  ;
         }
         */
        
        char d ;
        int byteIndex = 0 ;
        int imapIndex = nodeNum ;
        
        for(int i = 0 ; i < newFileSize ; i ++ )
        {
            d = newFile[i];
            workingBlock.byte[byteIndex] = d;
            byteIndex++;
            if (byteIndex == 1024)
            {
                
                Drive[driveSegIndex].SegSum.addDataBlockSS(imapIndex, Drive[driveSegIndex].blockIndex);
                Drive[driveSegIndex].saveBlock(workingBlock);
                SaveBlockIndexToInode(driveSegIndex, Drive[driveSegIndex].blockIndex-1);
                SetBlocksZero();
                byteIndex = 0;
                
            }
            
        }
        
        if (byteIndex%1024 != 0)
        {
            Drive[driveSegIndex].SegSum.addDataBlockSS(imapIndex, Drive[driveSegIndex].blockIndex);
            Drive[driveSegIndex].saveBlock( workingBlock);
            SaveBlockIndexToInode(driveSegIndex, Drive[driveSegIndex].blockIndex-1);
            byteIndex = 0;
        }
        
        Drive[driveSegIndex].SegSum.addInodeSS(imapIndex, Drive[driveSegIndex].blockIndex);
        Drive[driveSegIndex].saveInode(saveThisInode);
        
        
        imapFull[imapIndex] = driveSegIndex*1024 + Drive[driveSegIndex].blockIndex-1;
        
        int saveImapArray[256];
        int start ;
        start = imapIndex - (imapIndex % 256);
        
        for (int i = 0; i < 256; i ++)
        {
            saveImapArray[i] = imapFull[start + i] ;
        }
        Drive[driveSegIndex].SegSum.addImapSS(Drive[driveSegIndex].blockIndex);
        Drive[driveSegIndex].saveImap(saveImapArray);
        
        CPR.updateMapPart(imapIndex/256, driveSegIndex*1024 + Drive[driveSegIndex].blockIndex - 1);
        
        
        
        
        
        
    }
    else
    {

        int OBI = start/1024;
        int writingOffset = (ourInode.datablocks[OBI]%1024)*1024 + start%1024;
        //cout << "DATA BLOCK: " << OBI << ": " << ourInode.datablocks[OBI] << endl;
        //cout << "WRITING OFFSET: " << writingOffset << endl;
        //cout << "SEGMENT: " << ourInode.datablocks[OBI]/1024 << endl;
        OBI++;
        for (int i = 0; i < howmany; i++)
        {
            Drive[ourInode.datablocks[OBI-1]/1024].bytes[writingOffset] = c;
            writingOffset++;
            if (OBI%1024 == 0)
            {
                writingOffset = ourInode.datablocks[OBI];
                OBI++;
            }
            
        }
    }
    
}


void Directory::cat(string LFSfilename)
{
    ifstream infile("DRIVE/FILENAMEMAP");
    string compFile ;
    inode ourInode;
    imap ourImap;
    string colon;
    int nodeNum = 0;
    int offset;
    int bytesInFile = 0;
    int InodeNumber = 0;
    int segIndexList = 0 ;
    bool inFile = false;
    while(infile >> compFile >> colon >> nodeNum)
    {
        if(compFile == LFSfilename)
        {
            inFile = true;
            InodeNumber = nodeNum;
            break;
        }
    }
    if(!inFile)
    {
        cout << "ERROR: FILE NOT FOUND\n";
        return;
    }
    segIndexList = imapFull[InodeNumber] / 1024;
    offset = (imapFull[InodeNumber] % 1024 ) * 1024;
    memcpy(&ourInode, Drive[segIndexList].bytes + offset, sizeof(inode));
    bytesInFile = ourInode.filesize;
    block_struct block;
    for(int i=0;i<MAX_BLOCKS_PER_FILE;i++)
    {
        if(ourInode.datablocks[i] != 0)
        {
            segIndexList = ourInode.datablocks[i]/1024;
            offset = (ourInode.datablocks[i]%1024) * 1024;
            memcpy(&block,Drive[segIndexList].bytes + offset,1024);
            for(int j=0;j<1024;j++)
            {
                cout << block.byte[j];
                bytesInFile--;
                if(bytesInFile == 0)
                {
                    break;
                }
            }
        }
    }
    
    
    
}

void Directory::Display(string LFSfilename, int howmany, int start)
{
    ifstream infile("DRIVE/FILENAMEMAP");
    string compFile ;
    inode ourInode;
    imap ourImap;
    string colon;
    int counterBytes = howmany;
    int nodeNum = 0;
    int offset;
    int InodeNumber = 0;
    int segIndexList = 0 ;
    bool firstPass = false;
    
    while(infile >> compFile >> colon >> nodeNum)
    {
        if(compFile == LFSfilename)
        {
            InodeNumber = nodeNum;
            break;
        }
    }
    segIndexList = imapFull[InodeNumber] / 1024;
    offset = (imapFull[InodeNumber] % 1024 ) * 1024;
    memcpy(&ourInode, Drive[segIndexList].bytes + offset, sizeof(inode));
    block_struct block;
    int blockNumber = start/1024;
    offset = start % 1024;
    
    for(int i=blockNumber;i<128;i++)
    {
        //if(blockNumber == ourInode.datablocks)
        memcpy(&block,Drive[segIndexList].bytes + (ourInode.datablocks[i]%1024)*1024,1024);
        if(firstPass == false)
        {
            for(int j=offset;j<1024;j++)
            {
                if(counterBytes == 0)
                    return;
                cout << block.byte[j];
                counterBytes --;
            }
            firstPass = true;
        }
        else
        {
            
            
            for(int j=0;j<1024;j++)
            {
                if(counterBytes == 0)
                    return;
                else
                {
                    cout << block.byte[j];
                }
                counterBytes --;
            }
        }
    }
}

void Directory::Clean(int amountToClean)
{
    Drive[driveSegIndex].saveToDrive();
    int start;
    int saveImapArray[256];
    int imapPieceLoc;
    vector<int> imapVec;
    vector<int> inodeNumVec;
    vector<inode> inodeVec;
    int allInodeLoc[10000];

    int oldBlockNum;
    int writeBlockCount = 0;
    int segmentsToClean[amountToClean];
    int cleanArrayIndex = 0;
    int trashInt;

    bool changedData = false;
    bool inVector = false;
    //LOOKING FOR THE FIRST N USED SEGMENTS TO CLEAN
    for (int i = 0; i < 32; i++)
    {
        if (CPR.isUsed(i))
        {
            segmentsToClean[cleanArrayIndex] = i;
            cleanArrayIndex++;
            //Drive[segmentsToClean[cleanArrayIndex]].freeBlocks = 1016;
            if (cleanArrayIndex == amountToClean)
            {
                i = 50;
            }
        }
    }


    for(int i=0; i < amountToClean; i++)
    {
        for(int j=0;j<1024;j++)
        {

            if(Drive[segmentsToClean[i]].SegSum.locArray[j] > 9999 && Drive[segmentsToClean[i]].SegSum.locArray[j] < 40000)
            {
                memcpy(saveThisInode.datablocks,&imapFull[Drive[segmentsToClean[i]].SegSum.locArray[j] - 10000] , 1024);
                for (int k = 0; k < 128; k++)
                {
                    if (saveThisInode.datablocks[k] == j + segmentsToClean[i] * 1024)
                    {
                        inVector = false;
                        //THEN THIS DATA BLOCK IS LIVE, DATA SAVING IS CORRECT
                        if (inodeNumVec.size() > 0)
                        {
                        for (int l = 0 ; l < inodeNumVec.size(); l++)
                        {
                            if ((Drive[segmentsToClean[i]].SegSum.locArray[j]) - 10000 == inodeNumVec[l] )
                            {
                                inVector = true;
                                break;
                            }
                        }
                        }
                        if (!inVector)
                        {
                            cout <<"BEING PUSHED BACK: " << Drive[segmentsToClean[i]].SegSum.locArray[j] - 10000 << endl;
                            inodeNumVec.push_back( Drive[segmentsToClean[i]].SegSum.locArray[j] - 10000);
                        }
                    }
                }
                
            }
        
            
            
            else if (Drive[segmentsToClean[i]].SegSum.locArray[j] > 0 && Drive[segmentsToClean[i]].SegSum.locArray[j] < 9999)
            {

                if (imapFull[Drive[segmentsToClean[i]].SegSum.locArray[j]] == (j + segmentsToClean[i]*1024))
                {
                    inVector = false;
                    for (int l = 0 ; l < inodeNumVec.size(); l++)
                    {
                        if ((Drive[segmentsToClean[i]].SegSum.locArray[j]) == inodeNumVec[l])
                        {
                            inVector = true;
                            break;
                        }
                    }
                    if (!inVector)
                    {
                        inodeNumVec.push_back( Drive[segmentsToClean[i]].SegSum.locArray[j]);
                    }
                    //THIS INODE IS LIVE
                }
            }
        
            //cout <<"D: " << Drive[segmentsToClean[i]].SegSum.locArray[j];
            else if (Drive[segmentsToClean[i]].SegSum.locArray[j] == -2)
            {

                for(int k = 0; k < 40; k++)
                {
                    memcpy(&trashInt, CPR.bytes + 4*k, 4);
                    

                   if ( trashInt == segmentsToClean[i]*1024 + j )
                    {

                        inVector = false;
                        if (imapVec.size() > 0)
                        {
      
                            for (int v = 0; v < imapVec.size(); v++)
                            {
                                if (imapVec[v] == trashInt)
                                {
                                    inVector = true;
                                    break;
                                }
                            }
                        }
                        if (!inVector)
                        {
                            imapVec.push_back(trashInt);
                        }
                        //THIS MAP IS LIVE
                    }
                }
            }
        }

        
    }
    //IF THE DATA BLOCKS FOR THE INODE IS IN THE SEGMENTS, PUT IT IN THE WRITING BUFF
    driveSegIndex = CPR.getFreeSeg() ;
    for (int i = 0; i < inodeNumVec.size(); i++)
    {
            SetInodeZero();
            SetBlocksZero();
        
            if (Drive[driveSegIndex].freeBlocks < 0)
            {
                Drive[driveSegIndex].saveToDrive();
                cout << "DATA NEW SEG: " << driveSegIndex << endl;
                
    
                driveSegIndex = CPR.getFreeSeg();
                
                Segment newSeg(driveSegIndex);
                Drive.erase(Drive.begin() + driveSegIndex);
                Drive.insert(Drive.begin()+driveSegIndex, newSeg);
                CPR.changeLive(driveSegIndex) ;
            }
            //start a new segment

        
        
        
        memcpy(&saveThisInode, Drive[imapFull[inodeNumVec[i]]/1024].bytes + (imapFull[inodeNumVec[i]]%1024) * 1024, 1024);
        for (int j = 0; j < 128; j++)
        {
            changedData = false;
            trashInt = saveThisInode.datablocks[j];
            for (int k = 0 ; k < amountToClean; k++)
            {
                if (trashInt/1024 == segmentsToClean[k])
                {

                    memcpy(workingBlock.byte, Drive[trashInt/1024].bytes + (trashInt%1024 * 1024), 1024);
                            
                    Drive[driveSegIndex].SegSum.addDataBlockSS(inodeNumVec[i], Drive[driveSegIndex].blockIndex);
                    Drive[driveSegIndex].saveBlock(workingBlock);
                    saveThisInode.datablocks[j] = driveSegIndex*1024 + Drive[driveSegIndex].blockIndex-1;
                    changedData = true;
                    if (Drive[driveSegIndex].freeBlocks == 0)
                    {
                        Drive[driveSegIndex].saveToDrive();
                        
                        
                        
                        
                        driveSegIndex = CPR.getFreeSeg();
                        Segment newSeg(driveSegIndex);
                        Drive.erase(Drive.begin() + driveSegIndex);
                        Drive.insert(Drive.begin() + driveSegIndex, newSeg);
                        CPR.changeLive(driveSegIndex) ;
                    }
                    //HAVE TO HARD CODE SaveBlockIndexToInode(driveSegIndex, Drive[driveSegIndex].blockIndex-1);
                    SetBlocksZero();
                            
                }
            }

            Drive[driveSegIndex].SegSum.addInodeSS(inodeNumVec[i], Drive[driveSegIndex].blockIndex);
            Drive[driveSegIndex].saveInode(saveThisInode);
            
            //UpdateFNM(inodeNumVec[i], LFSfilename);
            imapFull[inodeNumVec[i]] = driveSegIndex*1024 + Drive[driveSegIndex].blockIndex-1;
            
            //finding imap piece
            imapPieceLoc = inodeNumVec[i] / 256;
            
            
            memcpy( &imapPieceLoc, CPR.bytes + imapPieceLoc, 4);
            memcpy(&saveThisImap, Drive[imapPieceLoc/1024].bytes + (imapPieceLoc % 1024)*1024, 1024);
            CPR.updateMapPart( inodeNumVec[i]/256, Drive[driveSegIndex].blockIndex);
            saveThisImap.inodeAndBlockNums[inodeNumVec[i]%256] = Drive[driveSegIndex].blockIndex;
            
            if (Drive[driveSegIndex].freeBlocks < 0)
            {
                Drive[driveSegIndex].saveToDrive();
                
                
                
                //cout << "IMAP NEW SEG: " << driveSegIndex << endl;
                driveSegIndex = CPR.getFreeSeg();
                //cout << "IMAP NEW SEG: " << driveSegIndex << endl;
                Segment newSeg(driveSegIndex);
                
                
                Drive.erase(Drive.begin() + driveSegIndex);
                Drive.insert(Drive.begin()+driveSegIndex, newSeg);
                
                CPR.changeLive(driveSegIndex) ;
            }
            
            start = inodeNumVec[i] - (inodeNumVec[i] % 256);

            for (int i = 0; i < 256; i ++)
            {
                saveImapArray[i] = imapFull[start + i] ;
            }
            Drive[driveSegIndex].SegSum.addImapSS(Drive[driveSegIndex].blockIndex);
            Drive[driveSegIndex].saveImap(saveImapArray);
                    
            CPR.updateMapPart(inodeNumVec[i]/256, driveSegIndex*1024 + Drive[driveSegIndex].blockIndex - 1);


                    //put in write buff
        }
    }
    
    for (int i = 0; i < amountToClean; i++)
    {
        for (int k = 0; k < 1024*1024; k++)
        {
            Drive[segmentsToClean[i]].bytes[k] = '0';
        }
        CPR.changeLive(segmentsToClean[i]);
        Drive[segmentsToClean[i]].saveToDrive();
    }
    
    Drive[driveSegIndex].saveToDrive();
}

    
    
    
    //FIND THE INODE FROM THE IMAP,
        //IF INODE IS IN THE SEGs
            //TAKE DATA BLOCKS, PUT THEM IN THE WRITE BUFFER
        //ONCE WRITE BUFFER IS FULL, WRITE DATA BLOCKS, UPDATE INODE, DROP INODE, UPDATE IMAP.
        //UPDATE SEG SUM AS YOU GO
            //ONCE YOU DROP MAX INODES PER IMAP, DROP AN IMAP PIECE, UPDATE CPR