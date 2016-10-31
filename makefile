 CXXFLAGS=-Wall -g -I. -pthread
all: awget ss

awget: awget.o common.o
	g++ $(CXXFLAGS) -o awget awget.o common.o
awget.o: awget.cpp awget.h
	g++ $(CXXFLAGS) -c awget.cpp
ss: ss.o common.o
	g++ $(CXXFLAGS) -o ss ss.o common.o
ss.o: ss.cpp awget.h
	g++ $(CXXFLAGS) -c ss.cpp
common.o: common.cpp common.h
	g++ $(CXXFLAGS) -c common.cpp
clean:
	-rm -f awget ss *.o

