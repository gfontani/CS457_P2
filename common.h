//Ben Scott, Chad Eitel, Gabriella Fontani
//CS457 Project2
//Dr. Indrajit Ray
//10/27/2016

#ifndef _COMMON_H_INCLUDED_   // if hasn't been included yet...
#define _COMMON_H_INCLUDED_   // #define this so the compiler knows it has been included

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <signal.h>
#include <arpa/inet.h>
#include <time.h>       /* time for seeding rand*/
#include <awget.h>
#include <sstream>
using std::stringstream;
#include <stdio.h>
using std::string;
#include <iostream>
using std::cout;
using std::cerr;
using std::endl;
using std::cin;

//global variables
extern int sockfd;

//function declarations: common functions needed for both awget and ss
void check_number(char* str);
void check_range(char* value, int min, int max);
void check_address(char* address);
void sig_handler(int signo);
void error(char const * msg);
int client_connect(char* addr, int portno);
void send_msg(int sock, packet* to_send);
void recv_msg(int sock, packet* to_recv);
int random_int(int max);
char* get_filename(const char* URL);
char* get_url_from_packet(packet* parsePacket);
char* get_chainList_from_packet(packet* parsePacket);
int pick_ip(packet* the_packet, char* ip);

#endif
