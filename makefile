 CXXFLAGS=-Wall -g -I.
all: awget ss

awget: awget.o
	g++ $(CXXFLAGS) awget.o -o awget
awget.o: awget.cpp awget.h
	g++ $(CXXFLAGS) -c awget.cpp
ss: ss.o
	g++ $(CXXFLAGS) ss.o -o ss
ss.o: ss.cpp awget.h
	g++ $(CXXFLAGS) -c ss.cpp
clean:
	-rm -f awget ss *.o

