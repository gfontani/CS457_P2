//Ben Scott, Chad Eitel, Gabriella Fontani
//CS457 Project2
//Dr. Indrajit Ray
//10/27/2016

#ifndef _AWGET_H_INCLUDED_   // if hasn't been included yet...
#define _AWGET_H_INCLUDED_   // #define this so the compiler knows it has been included

#define MAX_CHUNK_SIZE 34000	//this value should be <= the maximum size allowed through a socket connection in one "send"

//packet structure for url+chain packet
//example: url_len=14,chain_sz=3 data="www.google.com123.21.1,3600,123.45.2,3601,123.67.3,3602"
typedef struct{
	short data_sz; //size of incoming file in bytes (used to allocate memory for incoming file)
	short chunk_no; //use this number to memcopy to the correct place in memory for large file (chunk_no * MAX_CHUNK_SIZE)
	//short chunk_sz; //<--not necessary if we just allocate the same size memory for each packet (MAX_CHUNK_SIZE)
	short url_len; //url_len == 0 would indicate that msg is a file and not a chain packet)
	short chain_sz; //if url_len != 0, then chain_sz == 0 indicates last SS in chain)
	char* data; //nbytes
} packet;

#endif
