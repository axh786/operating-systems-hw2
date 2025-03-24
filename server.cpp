/*
Ali Husain
Dr. Rincon
COSC 3360: Programming Assignment 2
22 Mar 2025
*/
/*
have to fork in server.cpp
we will read the data in order that is sent to us from the client then implement the algorithm to parse and process the data

sendStringData(ptr->ranges, sockfd); // send ranges over to server
sendMatrixData(ptr->outputMatrix[ptr->index], ptr->cols, sockfd); // send the row that will be edited to the server
sendIntarrData(ptr->dataPos, ptr->dataSize, sockfd); // sending datapos
sendIntarrData(ptr->headPos, 2, sockfd); // sending headpos
readData(ptr->index, ptr->outputMatrix, sockfd); // read data from the server


*/
#include <unistd.h>
#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <strings.h>

/*void matrixEncoder() { algorithm

}*/

int main(int argc, char *argv[]) {
    
    return 0;
}