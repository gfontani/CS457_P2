//Ben Scott, Chad Eitel, Gabriella Fontani
//CS457 Project2
//Dr. Indrajit Ray
//10/27/2016

#include <awget.h>
#include <common.h>

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

	if (argc == 1) {
		server_accept_listen(3360);
	}
	else if(argc == 3){
		int portno = getPort(argc, argv);
		server_accept_listen(portno);
	}
	else{
		printf("*help*\n");
		printf("leave arguments blank or use -p [portno] for stepping-stone to listen on. Use ^c to exit.\n");
		printf("Exiting help.\n");
		exit(0);
	}
	
	packet to_recv;
	recv_msg(newsockfd, &to_recv);
	cout << "size1: " << to_recv.size1 << endl;
	cout << "size2: " << to_recv.size2 << endl;
	printf("data: %s",to_recv.data);
	printf("data2: %s", to_recv.data + to_recv.size1);

	wget_url("https://upload.wikimedia.org/wikipedia/en/c/cb/Wget.png");

	return 0;
}
