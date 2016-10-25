//Ben Scott, Chad Eitel, Gabriella Fontani
//CS457 Project2
//Dr. Indrajit Ray
//10/27/2016

#include <awget.h>
#include <common.h>

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

//forward file packets from sock1 to sock2 without actually saving file to SS
void file_forward(int sock1, int sock2){
	packet to_forward;
	recv_msg(sock1, &to_forward);
	int total_chunks = to_forward.size2;
	printf("total incoming chunks: %d\n", total_chunks);
	printf("chunk_no: 1, size: %d...\n", to_forward.size1);
	send_msg(sock2, &to_forward);
	for(int i=1; i<total_chunks; i++){
		recv_msg(sock1, &to_forward);
		printf("chunk_no: %d, size: %d...\n", i, to_forward.size1);
		send_msg(sock2, &to_forward);
	}
}

//send file to socket in chunks
void file_send(const char* filename, int sockfd){
	FILE *fileptr;
	long filelen;

	//get file length
	fileptr = fopen(filename, "rb");        
	fseek(fileptr, 0, SEEK_END);          
	filelen = ftell(fileptr);            
	rewind(fileptr);

	int total_chunks = filelen/MAX_CHUNK_SIZE + 1;
	printf("total_packets: %d\n", total_chunks);

	packet to_send;
	long remaining = filelen;
	for(int i=0; i<total_chunks; i++){
		//all complete chunks
		to_send.size2 = total_chunks; //total_chunks
		if(remaining >= MAX_CHUNK_SIZE){
			to_send.size1 = MAX_CHUNK_SIZE; //chunk_size
			fread(to_send.data, 1, MAX_CHUNK_SIZE, fileptr);
			remaining -= MAX_CHUNK_SIZE;
		}
		//last partial chunk
		else if(remaining < MAX_CHUNK_SIZE && remaining > 0){
			to_send.size1 = (int)remaining; //chunk_size
			bzero((char *) to_send.data, MAX_CHUNK_SIZE); //fill packet data with zeros since chunk will not fill entire packet
			fread(to_send.data, 1, (int)remaining, fileptr); //fill first part of packet data with remaining chunk
		}
		send_msg(sockfd, &to_send);
	}
}

//wget file: returns a pointer to the first byte of the file
void wget_url(const char* URL){
	int n;
	const char* command = "wget ";
	char str[strlen(command) + strlen(URL)];
 	strcpy(str, command);
	strcat(str, URL);
	n = system(str); //downloads file to working directory
	if(n<0)
		error("ERROR using wget");
}

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

//server setup
int server_accept_listen(int portno){
	char const * port_number = "3360"; //starting port

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

	printf("Found a friend! Waiting to receive...\n");
	return newsockfd;
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

int main(int argc, char* argv[])
{
	if (signal(SIGINT, sig_handler) == SIG_ERR){
		printf("\ncan't catch SIGINT\n");
		exit(1);
	}
	
	printf("****SS****\n");
	
//check arguments
	int portno = 0;
	if (argc == 1) {
		portno = 3360;
	}
	else if(argc == 3){
		portno = getPort(argc, argv);
	}
	else{
		printf("*help*\n");
		printf("leave arguments blank or use -p [portno] for stepping-stone to listen on. Use ^c to exit.\n");
		printf("Exiting help.\n");
		exit(0);
	}
	
	server_accept_listen(portno);//comment out when commenting gabby's code back in
/*
	//loop?
	while(true){
		//listen for connections
		server_accept_listen(portno);
		
		//recv chainfile packet
		packet to_recv;
		recv_msg(newsockfd, &to_recv);
		cout << "size1: " << to_recv.size1 << endl;
		cout << "size2: " << to_recv.size2 << endl;
		printf("data: %s",to_recv.data);
		printf("data2: %s", to_recv.data + to_recv.size1);
		char* url = get_url_from_packet(&to_recv);

		//if chainlist empty 
		if(0 == to_recv.size2){
			//get and send file
			wget_url(url);
			printf("file_send...\n");
			//Do we need to get the file name??
			char* filename = get_filename(url);
			file_send(filename, newsockfd);
			printf("file_send complete.\n");
		}
		else{
			//make new chainfile packet
			packet to_send;
			to_send.size1 = to_recv.size1;
			to_send.size2 = to_recv.size2 - 1;
			memcpy(to_send.data, url, strlen(url) + 1);
			
			char* chain_list = get_chainList_from_packet(&to_recv);
			char newList[MAX_CHUNK_SIZE - to_send.size1];
			int ipSize = 15;
			char ip[ipSize];
			int port = pick_ip(5, chain_list, newList, ip);
			
			strcat(to_send.data, newList);
		
			//send new chainfile packet
			
			//listen for and receive file cunks and send to previous ss
			
		}
		//teardown connection
	
	}
	//end loop?
*/
	
	
	packet to_recv;
	recv_msg(newsockfd, &to_recv);
	cout << "size1: " << to_recv.size1 << endl;
	cout << "size2: " << to_recv.size2 << endl;
	printf("data: %s",to_recv.data);
	printf("data2: %s", to_recv.data + to_recv.size1);

	wget_url("https://upload.wikimedia.org/wikipedia/en/c/cb/Wget.png");
	printf("file_send...\n");
	file_send("Wget.png", newsockfd);
	printf("file_send complete.\n");
	
	/*
	 * This was Gabby testing the random ip getter.  leaving it here in case we want it!
	char test[] = "1.2.3.4,1111,2.3.4.5,2222,3.4.5.6,3333,4.5.6.7,4444,5.6.7.8,5555";
	int ipSize = 15;
	char ip[ipSize];
	char newList[MAX_CHUNK_SIZE];
	printf("made all the variables\n");
	int port = pick_ip(5, test, newList, ip);
	printf("port: %d \n ip: %s \n newList: %s \n", port, ip, newList);
	*/
	return 0;
}
