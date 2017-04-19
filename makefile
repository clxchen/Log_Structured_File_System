# Martin LoBiondo, Nick Seney, Matt Myers 


all:	lab7

lab7: lab7.o Directory.o CheckPointRegion.o Segment.o SegmentSummary.o
	g++ -std=c++11 -g lab7.o Directory.o CheckPointRegion.o Segment.o SegmentSummary.o -o lab7

lab7.o: lab7.cpp
	g++ -std=c++11 -c -g lab7.cpp

Directory.o: Directory.cpp
	g++ -std=c++11 -c -g Directory.cpp

Segment.o: Segment.cpp
	g++ -std=c++11 -c -g Segment.cpp

CheckPointRegion.o: CheckPointRegion.cpp
    g++ -std=c++11 -c -g CheckPointRegion.cpp

SegmentSummary.o: SegmentSummary.cpp
	g++ -std=c++11 -c -g SegmentSummary.cpp
#DIR


DIR: DIR.o
	g++ -std=c++11 -g DIR.o -o DIR

DIR.o: DIR.cpp
	g++ -std=c++11 -c -g DIR.cpp

clean: 
	rm -f *.o lab7
	rm -Rf DRIVE/
