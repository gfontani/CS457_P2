//Ben Scott, Chad Eitel, Gabriella Fontani
//CS457 Project2
//Dr. Indrajit Ray
//10/27/2016

#ifndef _AWGET_H_INCLUDED_   // if hasn't been included yet...
#define _AWGET_H_INCLUDED_   // #define this so the compiler knows it has been included

#define MAX_CHUNK_SIZE 65000	//this value should be <= the maximum size allowed through a socket connection in one "send"

//packet structure for url+chain packet
//example: url_len=14,chain_sz=3 data="www.google.com" data2="123.21.1,3600,123.45.2,3601,123.67.3,3602"
typedef struct{
	short size1; //URL length or chunk_no
	short size2; //either chain length or total_chunks
	char* data1; //URL or chunk
	char* data2; //either chainlist or null
} packet;

#endif
