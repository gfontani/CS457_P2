//Ben Scott, Chad Eitel, Gabriella Fontani
//CS457 Project2
//Dr. Indrajit Ray
//10/27/2016

#include <common.h>

//global variables
int sockfd;
int newsockfd;

//sanity chex
void check_number(char* str){
	char c;
	for(int i=0; i<(int)strlen(str); i++){
		c = str[i];
		if((c-'0')<0 || (c-'0')>9){
			printf("ERROR: '%s' contains a non-integer value. Please try agian.\n", str);
			exit(0);
		}	
	}
}


//check range
void check_range(char* value, int min, int max){
	check_number(value);
	int i = atoi(value);
	if(i<min || i>max){
		printf("ERROR: value '%s' out of range. please use value in range (%d-%d)\n", value, min, max);
		exit(0);
	}
}

//check valid address
void check_address(char* address){
	int count = 0;
	int curr = 0;
	for(int next=1 ; next<=(int)strlen(address);){
		//find first '.'
		if(address[next] == '.' || next==(int)strlen(address)){//found next dot
			char input[256];
			bzero(input,256);
			for(int i=0; i<next-curr; i++){
				input[i]=address[i+curr];
			}
			check_range(input, 0, 255);
			count++;
			curr=next+1;	
		}
		if(count>4){
			printf("ERROR: address '%s' contains too many values. IP must be 32 bit/ 4 numbers 0-255 separated by dots.\n", address);
			exit(0);
		}
		
		next = next+1;
	}
	if(count<4){
		printf("ERROR: address '%s' contains too few values (%d). IP must be 32 bit/ 4 numbers 0-255 separated by dots.\n", address, count);
		exit(0);
	}
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
void error(char const * msg)
{
	perror(msg);
	close(newsockfd);
	close(sockfd);
	exit(1);
}

//client setup
//args: addr and port to connect to 
int client_setup(char* addr, int portno){
	struct sockaddr_in serv_addr;
	struct hostent *server;

	printf("Connecting to server... ");

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) error("ERROR opening socket");

	server = gethostbyname(addr);
	if (server == NULL) {
		fprintf(stderr,"ERROR, no such host\n");
		exit(0);
    }
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
	serv_addr.sin_port = htons(portno);
	if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
		error("ERROR connecting");

	printf("Connected!\n");
	printf("Connected to a friend! Sending...\n");
	return sockfd;
}
