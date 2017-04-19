//
//  CheckPointRegion.cpp
//  
//
//  Created by Matthew Myers on 5/3/16.
//
//

#include "CheckPointRegion.h"

CheckPointRegion::CheckPointRegion()
{
//	cout << "in constructor" << endl ;

	for (int i = 0 ; i < 193 ; i ++){
		bytes[i] = '0' ;
	}

	liveSeg = 0 ;
	liveMaps = 0 ;

	for(int i = 0 ; i < 40; i++){
		isMapLocUsed[i] = 0 ;
	}
	
}

void CheckPointRegion::UpdateCPR(char * readIn){

	liveSeg = 0 ;
	liveMaps = 0 ;
	
	for(int i = 0 ; i < 192 ; i++){
		bytes[i] = readIn[i] ;
	}

	for(int i = 161 ; i < 192 ; i++){
		if(readIn[i] == '1'){
			liveSeg ++ ;
		} 
	}

	int firstFree ;

	for(int i = 0 ; i < 161 ; i ++){
		if(readIn[i] == '0'){
			firstFree = i /4 ;
			for(int j = 0 ; j  < firstFree ; j++){
				isMapLocUsed[j] = 1 ;
				liveMaps ++ ;
			} 
			break ;
		}
	}

}


int * CheckPointRegion::arrayOfImapLocations()
{
	int* imapLocations = new int[161/4];
	int j = 0;
	for(int i=0;i<161;i+=4)
	{
		memcpy(&imapLocations[j], bytes + i,  4);
		j++ ;
	}

	return imapLocations;

}
void CheckPointRegion::addImap(int blockNum)
{

	if(liveMaps < 41){
		for(int i = 0 ; i < 40; i++){
			if(isMapLocUsed[i] == 0){
				
				isMapLocUsed[i] = 1; 
				memcpy(bytes + (4 * i), &blockNum, sizeof(int)) ;
				break ;
			}
		}
	
	}
	
	int segNum = (blockNum - ( blockNum % 1024 ) ) / 1024 ;

	liveMaps ++ ;
	//cout << d << endl ;

}

void CheckPointRegion::changeLive(int segment)
{
	
	int change = segment + 161;


	if(bytes[change] == '0'){
		bytes[change] = '1' ;
		liveSeg ++ ;
	}

	else if(bytes[change] == '1'){
		bytes[change] = '0' ;
		liveSeg -- ;
	}    
/*
	for(int i = 161 ; i < 193; i++){
		cout << " " << bytes[i] ;
	}
*/
}

bool CheckPointRegion::isUsed(int segment)
{
    int seg = segment + 161 ;
	if(bytes[seg] == '0'){
		return false ;
	}

	else { return true ; }
}

int CheckPointRegion::getFreeSeg(){
	for(int i = 161 ; i < 192 ; i++){
		if(bytes[i] == '0'){
			return (i - 161) ;
		}
	}

}

void CheckPointRegion::updateMapPart(int mapPart, int blockNum)
{
	memcpy(bytes + (4 * mapPart), &blockNum, sizeof(int)) ;
	
}

void CheckPointRegion::updateFile(char * name){

	ofstream out(name) ;
	if( !out )
    {
		cout << "error opening file" << endl ;
	}

	for(int i = 0 ; i < 192 ; i++){
		out << bytes[i] ;
	}
}




