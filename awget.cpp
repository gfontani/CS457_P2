//Ben Scott, Chad Eitel, Gabriella Fontani
//CS457 Project2
//Dr. Indrajit Ray
//10/27/2016

#include <awget.h>
#include <common.h>

void readAfile(string url, string fileName){
  int i = 0;
  int j = 0;
  string line = "";
  ifstream myfile (fileName); //tries to open the file
  //**load url into struct**//
  for(unsigned int i = 0; i < url.size(); i++){
  data[j] = url.at(i); //loads url into packet
  j++;
  }
  
  
  if(myfile.is_open()) //checks to see if file exists and can be opened.
  { getline(myfile,line); //grabs first line, number of ss
    stringstream convert(line);
    convert>>size2; //convert string to short int.
    cout<<"size2 " << size2<<endl;
    while(getline (myfile,line) ){
      cout<< line<< '\n'; //prints out file contents... 
      /*need to print out chainFile and next ip at each step
       * probably need to do this from the struct
       */
      for(unsigned int i = 0; i < line.size(); i++){
        data[j] = line.at(i);
        if(data[j] == ' '){
          data[j] = ',';
        }
        j++;
      }
      data[j] = ',';
      j++;
    }
    //set last index to whatever terminator you want.
    data[j-1] = 0;
    myfile.close();
    cout<<"now for the array\n";
    
    /*prints out data[]*/
    while(data[i] != 0)//this only works if the array is calloced.
    {
      cout<<data[i]; 
      i++;
    }
    cout<<'\n';//new line at the end of array printout.
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
