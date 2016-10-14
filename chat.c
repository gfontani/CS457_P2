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

#define BUFF_SIZE 142
#define PACKET_SIZE 144

/* global variable declaration */
int sockfd;
int newsockfd;
int is_server;
int portno;
char* addr;
char buffer[PACKET_SIZE];

//sanity chex
void check_number(char* str){
	char c;
	for(int i=0; i<strlen(str); i++){
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


//check address
void check_address(char* address){
	int count = 0;
	int curr = 0;
	for(int next=1 ; next<=strlen(address);){
		//find first '.'
		if(address[next] == '.' || next==strlen(address)){//found next dot
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

//packet structure
typedef struct{
	short version; //2bytes
	short str_len; //2bytes
	char* message; //nbytes
} packet;

//print address of server
void printAddr(char* port){
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
		bzero(buffer,BUFF_SIZE);
		exit(0);
	}
	else{
		printf("\nunhandled signo, exiting forcefully\n");
		close(newsockfd);
		close(sockfd);
		bzero(buffer,BUFF_SIZE);
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

//get port number from args
int getPort(int argc, char *argv[]){
	for(int i=0; i<argc-1; i++){
		if(strcmp(argv[i],"-p")==0){
			check_range(argv[i+1], 1024, 65535);
			return atoi(argv[i+1]);
		}
	}
	return -1;
}

//get address from args
char* getAddr(int argc, char *argv[]){
	for(int i=0; i<argc-1; i++){
		if(strcmp(argv[i],"-s")==0){
			printf("address before: %s\n", argv[i+1]);
			check_address(argv[i+1]);
			printf("address after: %s\n", argv[i+1]);
			return argv[i+1];
		}
	}
	return NULL;
}

//check buffer size
int checkbuffer(char * buff, int max){
	for(int i=0; i<max; i++){
		if(buff[i] =='\n'){
			printf("found \\n @%d\n", i);
			printf("size: %d\n",i);
			return i;
		}
		if(buff[i] == '\0'){
			printf("found \\0 @%d\n",i);
		}
		else printf("found %c @%d\n", buff[i], i);
	}
	printf("checkbuffer exceeded buffer size of %d\n", max);
	return -1;
}

//send message
void send_msg(int sock){
		int n;
		int size;
		while(1){
			printf("You: ");
			bzero(buffer,BUFF_SIZE);
			fgets(buffer,BUFF_SIZE,stdin);
			//checkbuffer(buffer, BUFF_SIZE);o make the interfac
			//printf("buffer[bffsz-]:\t%c\n", buffer[BUFF_SIZE-1]);
			if(buffer[BUFF_SIZE-2]=='\0' || buffer[BUFF_SIZE-2]=='\n')
				break;
			printf("exceeded string length of %d. clearing stdin...\n", BUFF_SIZE-2);
			char ch;
			while((ch = getchar())!='\n'  && ch != EOF  );//clear stdin
		}



		short sz = 4 + (short)strlen(buffer)-1;
		//printf("min size of packet:\t%d\n",sz);

		char to_send[sz];
		short* header = (short*)to_send;
		header[0] = 457;
		header[1] = (short)strlen(buffer)-1;
		memcpy(to_send+4, buffer, strlen(buffer)-1);

		n = write(sock,to_send,sz);
			if (n < 0) error("ERROR writing to socket");
		bzero(buffer,BUFF_SIZE);
}

//recv message
void recv_msg(int sock){
	int n = read(sock,buffer,PACKET_SIZE);
	if (n < 0) error("ERROR reading from socket");

	short* header = (short*)buffer;
	char* begin = buffer;
	packet recvd;
	recvd.version = header[0];
	recvd.str_len = header[1];
	recvd.message = begin+4;

	if(recvd.str_len>140)
		recvd.str_len = 140;

	//printf("recvd ver: %d, str_len: %d, msg: ", recvd.version, recvd.str_len);
	printf("Friend: ");
	for(int i=0; i<recvd.str_len; i++){
		printf("%c",recvd.message[i]);
	}
	printf("\n");
}

//send_recv loop
void send_recv(int sock){
	while(1){
		send_msg(sock);
		recv_msg(sock);
	}
}

//server setup
void server_setup(){
	char * port_number = "3360"; //starting port
	portno = 3360;

	socklen_t clilen;
	struct sockaddr_in serv_addr, cli_addr;
	int n;
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
			error("ERROR on binding");
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

	printf("Found a friend! You receive first.\n");
	recv_msg(newsockfd);
	send_recv(newsockfd);
}

//client setup
void client_setup(){
    int n;
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
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");

	printf("Connected!\n");
	printf("Connected to a friend! You Send first.\n");
	
	send_recv(sockfd);
}


int main(int argc, char* argv[])
{
	if (signal(SIGINT, sig_handler) == SIG_ERR){
		printf("\ncan't catch SIGINT\n");
		exit(1);
	}

	//for(int i=0; i<argc; ++i){
		//printf("Argument %d : %s\n", i, argv[i]);
	//}

	if (argc == 1) {
		is_server = 1;
		printf("****server****\n");
		server_setup();
	}
	else if(argc == 5){
		is_server = 0;
		printf("****client****\n");
		portno = getPort(argc, argv);
		addr = getAddr(argc, argv);
		client_setup();
	}
	else{
		printf("*help*\n");
		printf("use -p [portno] -s [ipaddress] for client or leave arguments blank for server. Use ^c to exit running chat.\n");
		printf("Exiting help.\n");
		exit(0);
	}
	return 0;
}
