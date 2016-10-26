//Ben Scott, Chad Eitel, Gabriella Fontani
//CS457 Project2
//Dr. Indrajit Ray
//10/27/2016

#include <awget.h>
#include <common.h>

#include <stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>
#include<cstdlib>
#include<strings.h>
#include<sstream>
#include<string.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<iostream>
#include<arpa/inet.h>
#include<signal.h>
#include<csignal>
#include<fstream> //inclde for talking to files.

using namespace std;

void read_chainfile(string url, string fileName, packet* to_send){
  int i = 0; 
  int j = 0;
  string line = "";
  ifstream myfile (fileName); //tries to open the file
  //**load url into struct**//
  to_send->size1 = url.size();
  for(unsigned int i = 0; i < url.size(); i++){
  to_send->data[j] = url.at(i); //loads url into packet
  j++;
  }
  
  
  if(myfile.is_open()) //checks to see if file exists and can be opened.
  { getline(myfile,line); //grabs first line, number of ss
    stringstream convert(line);
	short chain_len = -1;
    convert>>chain_len; //convert string to short int.
    cout<<"size2 " << chain_len<<endl;
    to_send->size2 = chain_len;
    while(getline (myfile,line) ){
      cout<< line<< '\n'; //prints out file contents... 
      /*need to print out chainFile and next ip at each step
       * probably need to do this from the struct
       */
      for(unsigned int i = 0; i < line.size(); i++){
        to_send->data[j] = line.at(i);
        if(to_send->data[j] == ' '){
          to_send->data[j] = ',';
        }
        j++;
      }
      to_send->data[j] = ',';
      j++;
    }
    //set last index to whatever terminator you want.
    to_send->data[j-1] = '\0';
    myfile.close();
    cout<<"now for the array\n";
    
    /*prints out data[]*/
    while(to_send->data[i] != '\0')//this only works if the array is calloced.
    {
      cout<<to_send->data[i]; 
      i++;
    }
    cout<<"finished printing\n";//new line at the end of array printout.
  }
  else {
    cout<<"unalble to open file"<<'\n';
    
  }
}

void file_recv(int sock, FILE* file){
	packet recvd;
	recv_msg(sock, &recvd);
	int total_chunks = recvd.size2;
	printf("total incoming chunks: %d\n", recvd.size2);
	int written = fwrite((void*) &recvd.data[0], 1, recvd.size1, file);
	printf("chunk 0: %dB\n", written);
	for(int i=1; i<total_chunks; i++){
		recv_msg(sock, &recvd);
		written = fwrite((void*) &recvd.data[0], 1, recvd.size1, file);
		printf("chunk %d: %dB\n", i, written);
	}
}

int main(int argc, char* argv[]){
	if (signal(SIGINT, sig_handler) == SIG_ERR){
		printf("\ncan't catch SIGINT\n");
		exit(1);
	}

	printf("****awget****\n");
	
	packet to_send;
	read_chainfile("https://upload.wikimedia.org/wikipedia/en/c/cb/Wget.png", "chain.txt", &to_send);
	printf("size1: %d\n", to_send.size1);
	printf("size2: %d\n", to_send.size2);
	printf("data: %s\n", to_send.data);
	
	
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

	send_msg(sockfd, &to_send);

	FILE* fileptr;
	fileptr = fopen("newfile.png", "wb"); //create file "newfile" with mode write in bytes
 	if (fileptr==NULL){
		printf("unable to open file. exiting.\n");
		close(sockfd);
		return 0;
	}

    

	printf("file_recv...\n");
	file_recv(sockfd, fileptr);
	printf("file_recv finished.\n");

	fclose (fileptr);
	close(sockfd);


	return 0;
}
