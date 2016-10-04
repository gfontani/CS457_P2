//Ben Scott, Chad Eitel, Gabriella Fontani
//CS457 Project2
//Dr. Indrajit Ray
//10/27/2016

#ifndef _AWGET_H_INCLUDED_   // if hasn't been included yet...
#define _AWGET_H_INCLUDED_   // #define this so the compiler knows it has been included

//packet structure for url+chain packet
//example: url_len=14,chain_sz=3 data="www.google.com123.21.1,3600,123.45.2,3601,123.67.3,3602"
typedef struct{
	short url_len; //2bytes
	short chain_sz; //2bytes
	char* data; //nbytes
} chain_packet;

#endif
