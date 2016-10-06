//Ben Scott, Chad Eitel, Gabriella Fontani
//CS457 Project2
//Dr. Indrajit Ray
//10/27/2016

#include <awget.h>
#include <common.h>

int main(int argc, char* argv[]){
	if (signal(SIGINT, sig_handler) == SIG_ERR){
		printf("\ncan't catch SIGINT\n");
		exit(1);
	}

	printf("****awget****\n");
	
	if (argc == 2) {
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
	
	//TODO
	//sockfd = client_setup(chain_address, chain_portno);
	//send chain-gang to next ss
	//wait to receive file
	
	close(sockfd);
	return 0;
}
