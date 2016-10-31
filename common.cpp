//Ben Scott, Chad Eitel, Gabriella Fontani
//CS457 Project2
//Dr. Indrajit Ray
//10/27/2016

#include <common.h>

//global variable
int sockfd; //socket to listen on

//get the filename from the URL
char* get_filename(const char* URL){
	int index = -1;
	char c;
	for(int i=0; i<(int)strlen(URL); i++){
		c = URL[i];
		if(c=='/'){
			index = i; 
		}	
	}
	if(index > -1 && index<(int)strlen(URL)-1){
		return 	(char*)&URL[index+1];
	}
	else return (char*)"index.html";
}

//returns a random number between (inclusive) 0 and max
int random_int(int max){
	srand (time(NULL));
	int random_value = rand() % (max+1);
	printf("random value chosen: %d", random_value);
	return random_value;
}

//gets the url from the packet data
char* get_url_from_packet(packet* parsePacket){
	char* url = new char[parsePacket->size1 + 1];
	strncpy(url, parsePacket->data, parsePacket->size1);
	return url;
}

//gets the chainlist from the packet data
char* get_chainList_from_packet(packet* parsePacket){
	char* chain_list = new char[MAX_CHUNK_SIZE - parsePacket->size1];
	strncpy(chain_list, parsePacket->data + parsePacket->size1, MAX_CHUNK_SIZE - parsePacket->size1);
	return chain_list;
}

void print_chainfile(char* chain_list){
	//split all of the IPs
	//get the chosen ip and port
	//put the rest in the newList
	char* split = strtok(chain_list, ",");
	printf("chainlist is: \n");
	
	while(split != NULL){
		//prnit ip
		printf("%s, ", split);
		split = strtok(NULL, ","); 
		//print port
		printf("%s\n", split);
		split = strtok(NULL, ",");
	}
	
}

//picks a random IP from the chain file and removes the ip and 
//port from the list
//Modifies the packet so that it has the new list and the correct value
// for size2
//the_packet: the packet to read from and change
//ip is a reference to the next ip address.  It will be filled in
//returns: the chosen port
//
//NOTE: this function assumes that the list is in the correct format
int pick_ip(packet* the_packet, char* ip){
	
	//pick a random IP and remove it from the chain list
	char* chain_list = get_chainList_from_packet(the_packet);
	char newList[MAX_CHUNK_SIZE];
	int num_ips = the_packet->size2;
	int port = -1;
	//pick a random number
	int chosen_ip = random_int(num_ips -1);
	printf("random int: %d\n", chosen_ip);
	
	//split all of the IPs
	//get the chosen ip and port
	//put the rest in the newList
	int counter = 0;
	char* split = strtok(chain_list, ",");
				//printf("split: %s\n", split);

	while(split != NULL){
		if(chosen_ip == counter){
			strcpy(ip, split);
			split = strtok(NULL, ","); 
			port = atoi(split);
			split = strtok(NULL, ",");
		}
		else{
			if((0 == counter && 0 != chosen_ip) || (1 == counter && 0 == chosen_ip)){
				//add ip
				strcpy(newList, split);
				strcat(newList, ",");
				split = strtok(NULL, ","); 
				//add port to list
				strcat(newList, split);
				split = strtok(NULL, ",");
			}
			else{
				strcat(newList, ",");
				//add ip to list followed by a comma
				strcat(newList, split);
				strcat(newList, ",");
				split = strtok(NULL, ","); 
				//add port to list
				strcat(newList, split);
				split = strtok(NULL, ",");
			}
		}
		counter++;
	}
	
	//put new values into packet
	the_packet->size2 = the_packet->size2 - 1;
	if(0 == the_packet->size2){
		char* url = get_url_from_packet(the_packet);
		memcpy(the_packet->data, url, strlen(url) + 1);
	}
	else{
		memcpy(the_packet->data + the_packet->size1, newList, strlen(newList) + 1);
	}
	return port;
}

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
		close(sockfd);
		exit(0);
	}
	else{
		printf("\nunhandled signo, attempting to exit gracefully\n");
		close(sockfd);
		exit(-1);
	}
}

//print error message
void error(char const * msg)
{
	perror(msg);
	close(sockfd);
	exit(1);
}

//client setup
//args: addr and port to connect to 
int client_connect(char* addr, int portno){
	struct sockaddr_in serv_addr;
	struct hostent *server;

	printf("Connecting to server... ");

	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0) error("ERROR opening socket");

	server = gethostbyname(addr);
	if (server == NULL) {
		fprintf(stderr,"ERROR, no such host\n");
		exit(0);
    }
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
	serv_addr.sin_port = htons(portno);
	if (connect(sock,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
		error("ERROR connecting");

	printf("Connected!\n");
	printf("Connected to a friend! Sending...\n");
	return sock;
}

void send_msg(int sock, packet* to_send){
		int n;
		n = send(sock,reinterpret_cast<const char*>(to_send),sizeof(packet), 0);
			if (n < 0) error("ERROR writing to socket");
}

void recv_msg(int sock, packet* recvd){
	int n = recv(sock,reinterpret_cast<char*>(recvd),sizeof(packet), MSG_WAITALL);
	if (n < 0) error("ERROR reading from socket");
}
