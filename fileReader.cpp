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
//#define SERVER_PORT 5432
#define MAX_LINE 256
#define MAX_PENDING 5
using namespace std;


//ofstream i.e. output stream, write to a file
//ifstream i.e. input stream, read from a file
#define MAX_CHUNK_SIZE 1400
char data[MAX_CHUNK_SIZE];

/*Writes msg to the file that is passed in*/
void writeToAFile(string msg, string fileName){
  ofstream myStream;  //creates a file writer
  myStream.open(fileName);
  myStream<<msg; //Writes to the file
  myStream<<"\n";
  myStream<<"one more time. \n"<<msg<<endl;
  myStream.close();//don't forget to close your ofstream
  
}

//Reads a file and loads it into the struct
void readAfile(string url, string fileName){
  int i = 0;
  int j = 0;
  string line = "";
  ifstream myfile (fileName); //tries to open the file
  //**load url into struct**//
  for(unsigned int i = 0; i < url.size(); i++){
  data[j] = url.at(i);
  j++;
  }
  data[j] = ',';
  j++; //add j at the end of the url//
  
  if(myfile.is_open()) //checks to see if file exists and can be opened.
  {
    while(getline (myfile,line) ){
      cout<< line<< '\n'; //prints out file contents... 
      /*need to print out chainFile and next ip at each step
       * probably need to do this from the struct
       */
      for(unsigned int i = 0; i < line.size(); i++){
        data[j] = line.at(i);
        j++;
      }
      data[j] = ',';
      j++;
    }
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
//***Writes file to terminal, might need to do this from the struct...***///
//Not used//
void writeFileToTerminal(string fileName, string url){
  string line = "";
  cout<<url<<endl;
  ifstream myFile(fileName);
  if(myFile.is_open()){
    while(getline(myFile,line)){
      cout<<line<<endl;
    }
  }
  
}


void usage(){
  cout<<"usage <file to read>\n" ;
}



int main(int argc, char* argv[])
{
  string url = "";
  string fileToRead = "";
  printf("hello world, I need to read a file\n");
  if(argc < 2 || argc > 3){
    usage();
    printf("please enter a valid file name to read.\n");
    return 0;
  }
  if(argc==2){
    //string fileToWrite = argv[1];
    url = argv[1];
    fileToRead = "chainFile.txt"; //****hard coded file name***///
    //writeToAFile(msg, fileToWrite);
    readAfile(url, fileToRead);
    
  }
  if(argc==3){
    //string fileToWrite = argv[1];
    url = argv[1];
    fileToRead = argv[2]; //****hard coded file name***///
    //writeToAFile(msg, fileToWrite);
    readAfile(url, fileToRead);
  }
  return 0;
}
