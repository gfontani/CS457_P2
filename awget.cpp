//Ben Scott, Chad Eitel, Gabriella Fontani
//CS457 Project2
//Dr. Indrajit Ray
//10/27/2016

#include <awget.h>
#include <common.h>

void file_recv(int sock, FILE* file){
	packet recvd;
	recv_msg(sock, &recvd);
	//TODO create file using individual chunks
	int total_chunks = recvd.size2;
	printf("total incoming chunks: %d\n", recvd.size2);
	printf("chunck_no: %d\n", recvd.size1);
	for(int i=1; i<total_chunks; i++){
		recv_msg(sock, &recvd);
		//TODO create file using individual chunks
		printf("chunck_no: %d\n", recvd.size1);		
	}
}

int main(int argc, char* argv[]){
	if (signal(SIGINT, sig_handler) == SIG_ERR){
		printf("\ncan't catch SIGINT\n");
		exit(1);
	}

	printf("****awget****\n");
	
/*	if (argc == 2) {
		//URL is first argument with no flag
		printf("no chain file requested, will attempt to read chaingang.txt.\n");
		//TODO
		//attempt to read chaingang.txt
	}
	else if(argc == 4){
		//URL is first argument with no flag
		printf("chain file supplied, will attempt to read.\n");
		//TODO
		//attempt to read supplied chain-file
	}
	else{
		printf("*help*\n");
		printf("insert helpful words here.\n");
		printf("Exiting help.\n");
		exit(0);
	}
	*/
	
	//TODO
	//TODO change this!!!
	
	int portno;
	stringstream str(argv[1]);
	str >> portno;
	char* addr = argv[2];
	sockfd = client_connect(addr, portno);
	packet test;
	test.size1 = 2;
	test.size2 = 100;
	const char* hello = "hello world\n";
	//NOTE: the + 1 is necessary to avoid strange characters
	memcpy(test.data, hello, strlen(hello) + 1);
	send_msg(sockfd, &test);
	//send chain-gang to next ss
	//wait to receive file

	FILE* fileptr;
	fileptr = fopen("newfile", "wb"); //create file "newfile" with mode write in bytes

	printf("file_recv...\n");
	file_recv(sockfd, fileptr);
	printf("file_recv finished.\n");
	
	close(sockfd);


	return 0;
}
