#include "SegmentSummary.h"




SegmentSummary::SegmentSummary(){

	for( int i = 0 ; i < 1024 ; i ++){
		locArray[i] = -1 ;	
	}

	for(int i = 0 ; i < 1024*8 ; i++){
		charArray[i] = '0' ;
	}

}


void SegmentSummary::addInodeSS(int nodeNum, int loc){
	locArray[loc] = nodeNum ;

}


void SegmentSummary::addImapSS(int loc){
	locArray[loc] = -2 ;

}

void SegmentSummary::addDataBlockSS(int nodeNum, int loc){
	locArray[loc] = nodeNum + 10000 ;
}

char* SegmentSummary::writeToBlock(){

	memcpy(&charArray , locArray , 1024*8) ;
	return charArray ;


}
