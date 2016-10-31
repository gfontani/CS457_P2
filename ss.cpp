//Ben Scott, Chad Eitel, Gabriella Fontani
//CS457 Project2
//Dr. Indrajit Ray
//10/27/2016

#include <awget.h>
#include <common.h>

//forward file packets from sock1 to sock2 without actually saving file to SS
void file_forward(int sock1, int sock2){
	printf("Relaying file...\n");
	packet to_forward;
	recv_msg(sock1, &to_forward);
	int total_chunks = to_forward.size2;
	//printf("total incoming chunks: %d\n", total_chunks);
	//printf("chunk_no: 1, size: %d...\n", to_forward.size1);
	send_msg(sock2, &to_forward);
	for(int i=1; i<total_chunks; i++){
		recv_msg(sock1, &to_forward);
	//	printf("chunk_no: %d, size: %d...\n", i, to_forward.size1);
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
	//printf("total_packets: %d\n", total_chunks);

	remove(filename);

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
	
	//command is executed in this order...
	const char* command = "wget ";
	//URL
	const char* command_flag = " -O ";
	char* filename = get_filename(URL);
	
	
	char str[strlen(command) + strlen(URL) +strlen(command_flag) + strlen(filename)];
 	strcpy(str, command); //command
	strcat(str, URL); //URL
	strcat(str, command_flag); //flag
	strcat(str, filename); //filename
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

//server listen, returns listening socket fd
int server_bind_listen(int portno){
	int sock;
	char const * port_number = "3360"; //starting port
	struct sockaddr_in serv_addr;

	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0) error("ERROR opening socket");

	bzero((char *) &serv_addr, sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;

	//try 13 ports
	for(int i=0; i<13; i++){
		serv_addr.sin_port = htons(portno);
		if (bind(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) == 0)
			break;
		if(i==12)
			error("ERROR on binding"); //could not bind to 13 consecutive ports
		portno++;
	}

	listen(sock,5);
	printAddr(port_number);
	printf("port %d\n", portno);
	return sock;
}

//server setup, returns accepted socket fd
int server_accept(int sock){
	struct sockaddr_in cli_addr;
	socklen_t clilen;
	clilen = sizeof(cli_addr);
	int newsock = accept(sock, 
                 (struct sockaddr *) &cli_addr, 
                 &clilen);
	if (newsock < 0) error("ERROR on accept");

	//printf("Found a friend! Waiting to receive...\n");
	return newsock;
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

	//bind to port and listen for connections
	sockfd = server_bind_listen(portno);

	while(1){
		//parent thread blocks and waits for a new connection
		int newsock = server_accept(sockfd);

		//once connection is established, fork a child
		int pid = fork();

		//child does cool stuff
		if(pid==0){	
			//recv chainfile packet
			packet chain_packet;
			recv_msg(newsock, &chain_packet);

			char* url = get_url_from_packet(&chain_packet);
			printf("Request: %s\n", url);
			//if chainlist empty 
			if(0 == chain_packet.size2){
				char* filename = get_filename(url);
				printf("chainlist is empty\n");
				//get and send file
				printf("issueing wget for file <%s>\n", filename);
				wget_url(url);
				printf("File received\n");
				printf("Relaying file...\n");
				file_send(filename, newsock);
				printf("Finished relaying file. Goodbye!\n");
			}
			else{
				//print chainlist
				print_chainfile(get_chainList_from_packet(&chain_packet));
				//make new chainfile packet
				char ip[20];
				int port = pick_ip(&chain_packet, ip);
				//print next stepping stone
				printf("next SS is %s, %d\n", ip, port);
			
				//make new socket
				int forwardingSocket = client_connect(ip, port);
			
				//send new chainfile packet
				send_msg(forwardingSocket, &chain_packet);
			
				//forward the file
				printf("waiting for file...\n");
				file_forward(forwardingSocket, newsock);
				printf("Finished relaying file. Goodbye!\n");
				//teardown connection
				close(forwardingSocket);
			
			}
			//teardown connection
			close(newsock);
			return 0;
		}
		else if(pid<0){
			// fork failed
			printf("fork() failed!\n");
			close(sockfd);
			close(newsock);
			return 1;
		}
	}
	return 0;
}
