//Ben Scott, Chad Eitel, Gabriella Fontani
//CS457 Project2
//Dr. Indrajit Ray
//10/27/2016

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

int sockfd;
int newsockfd;
int is_server;
int portno;

//print address of server
void printAddr(char const* port){
	char hostname[100];
	gethostname(hostname,100);
	printf("hostname: %s\n", hostname);
	struct hostent* record = gethostbyname(hostname);
	struct in_addr * address = (struct in_addr * )record->h_addr;
	char *ip = inet_ntoa(* address);
	printf("Waiting for a connection on %s ", ip);
}

//signal handler for SIGINT
void sig_handler(int signo){
	if (signo == SIGINT){
		printf("\nreceived SIGINT, exiting gracefully\n");
		close(newsockfd);
		close(sockfd);
		exit(0);
	}
	else{
		printf("\nunhandled signo, attempting to exit gracefully\n");
		close(newsockfd);
		close(sockfd);
		exit(-1);
	}
}

//print error message
void error(const char *msg)
{
	perror(msg);
	close(newsockfd);
	close(sockfd);
	exit(1);
}

//server setup
void server_setup(){
	char const * port_number = "3360"; //starting port
	portno = 3360;

	socklen_t clilen;
	struct sockaddr_in serv_addr, cli_addr;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) error("ERROR opening socket");

	bzero((char *) &serv_addr, sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;

	//try 13 ports
	for(int i=0; i<13; i++){
		serv_addr.sin_port = htons(portno);
		if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) == 0)
			break;
		if(i==12)
			error("ERROR on binding"); //could not bind to 13 consecutive ports
		portno++;
	}
		
	printAddr(port_number);
	printf("port %d\n", portno);

	listen(sockfd,5);
	clilen = sizeof(cli_addr);
	newsockfd = accept(sockfd, 
                 (struct sockaddr *) &cli_addr, 
                 &clilen);
	if (newsockfd < 0) error("ERROR on accept");

	printf("Found a friend! Receiving chain-list first...\n");
	//get chain-list from prev client
	//if chain is not empty, connect to next SS and wait for file
	//else wget URL file
	//return file to prev client
}

int main(int argc, char* argv[])
{
	if (signal(SIGINT, sig_handler) == SIG_ERR){
		printf("\ncan't catch SIGINT\n");
		exit(1);
	}

	if (argc == 1) {
		is_server = 1;
		printf("****SS****\n");
		server_setup();
	}
	else{
		printf("*help*\n");
		printf("leave arguments blank for stepping-stone. Use ^c to exit.\n");
		printf("Exiting help.\n");
		exit(0);
	}
	return 0;
}
