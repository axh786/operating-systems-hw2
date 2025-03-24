/*
Ali Husain
Dr. Rincon
COSC 3360: Programming Assignment 2
22 Mar 2025
*/

#include <unistd.h>
#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <strings.h>
#include <vector>

struct row { // defining row struct from data given by input so each row will have the ranges to search thru, which head pos they are needed and then loop thru data pos
    char* ranges;
    int headPos[2];
    int* dataPos;
    int index;
    char** outputMatrix; // make changes to matrix when done with figuring out character placement
    
    char* serverIP;
    char* portno;
};



/*
Struct vectors will have to be changed into arrays
haed pos and data pos will be easily changed to arrays
ranges will be harder, maybe struct in struct?
output the matrix using strings and prints no more matrix maybe
where does lines come into play?
send char* ranges over there and find the ranges to use in the algorithm
*/

void createSocket(char* portNoRaw, char* serverIP, int sockfd) {
    // need to feed in sockfd portno and server IP
    int portno = std::atoi(portNoRaw);
    struct sockaddr_in serv_addr;
    struct hostent *server;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        std::cerr << "ERROR opening socket" << std::endl;
        exit(0);
    }
    server = gethostbyname(serverIP);
    if (server == NULL) {
        std::cerr << "ERROR, no such host" << std::endl;
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) {
        std::cerr << "ERROR connecting" << std::endl;
        exit(0);
    }
}

void sendStringData(const std::string& buffer, int sockfd) {
    int n;
    int msgSize = buffer.size();
    n = write(sockfd,&msgSize,sizeof(int));
    if (n < 0) {
        std::cerr << "ERROR writing to socket" << std::endl;
        exit(0);
    }
    n = write(sockfd,buffer.c_str(),msgSize);
    if (n < 0) {
        std::cerr << "ERROR writing to socket" << std::endl;
        exit(0);
    }
}

void sendIntarrData(int* arr, int sockfd) {
    int n;
    int arrSize = sizeof(arr)/sizeof(arr[0]);
    n = write(sockfd,&arrSize,sizeof(int));
    if (n < 0) {
        std::cerr << "ERROR writing to socket" << std::endl;
        exit(0);
    }
    n = write(sockfd,arr,sizeof(arr));
    if (n < 0) {
        std::cerr << "ERROR writing to socket" << std::endl;
        exit(0);
    }
}

void readData(int index, char** outputMatrix, int sockfd) {
    int msgSize, n;
    n = read(sockfd,&msgSize,sizeof(int));
    if (n < 0) {
        std::cerr << "ERROR reading from socket" << std::endl;
        exit(0);
    }
    char *tempBuffer = new char[msgSize+1];
    bzero(tempBuffer,msgSize+1);
    n = read(sockfd,tempBuffer,msgSize);
    if (n < 0) {
        std::cerr << "ERROR reading from socket" << std::endl;
        exit(0);
    }
    char* buffer = tempBuffer;
    delete [] tempBuffer;
    strcpy(outputMatrix[index], buffer);
}

void * decoder(void *void_ptr) {  // thread function
    int sockfd;
    /*
    What will be sent to the server thru this thread function:
    everything in rows will be sent im guessing so 4 different read/writes, first 5 attributes in my struct
    send a row of the matrix to the server -> using data and head pos aling with range will that row up 
    (we might not need to send index then) -> output it row by row
    */
    // create socket
    //write sendStringData, ranges
    //write sendStringData, row we are editng outputMatrix[index]
    //write sendIntarrData, dataPos
    //write sendIntarrData, headPos
    //read data
    close (sockfd);
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
       std::cerr << "usage " << argv[0] << " hostname port" << std::endl;
       exit(0);
    }

    int col, row; // reading matrix size from first line
    std::cin >> col >> row;
    std::cin.ignore();
    char** outputMatrix = new char*[row];
    
    for (int r = 0; r < row; ++r) { // initalizing 2d array
        outputMatrix[r] = new char[col];
        std::fill(outputMatrix[r], outputMatrix[r] + col, ' '); // fill 2d array with spaces before decoding
    }

    // read the rest out output, save ranges as a string, datapos and headpos as arrays and have an index
    // threading and outputting will both be done in main
    

    return 0;
}