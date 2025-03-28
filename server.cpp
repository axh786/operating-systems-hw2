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

void matrixEncoder(char* ranges, char* row, int* dataPos, int* headPos) { //algorithm

}

void clientInteraction(int newsockfd) {
    int n, msgSize = 0;
    n = read(newsockfd, &msgSize, sizeof(int)); // ranges of the chars
    if (n < 0) {
        std::cerr << "Error reading from socket" << std::endl;
        exit(0);
    }
    char* ranges = new char[msgSize+1];
    bzero(ranges, msgSize + 1);
    n = read(newsockfd, ranges, msgSize);
    if (n < 0) {
        std::cerr << "Error reading from socket" << std::endl;
        exit(0);
    }

    n = 0;
    int rowSize = 0; // matrix row that will be decoded
    n = read(newsockfd, &rowSize, sizeof(int));
    if (n < 0) {
        std::cerr << "Error reading from socket" << std::endl;
        exit(0);
    }
    char* row = new char[rowSize+1];
    bzero(row, rowSize + 1);
    n = read(newsockfd, row, rowSize);
    if (n < 0) {
        std::cerr << "Error reading from socket" << std::endl;
        exit(0);
    }

    int dataSize = 0; // dataPos arr that contains all of the positions in the matrix
    n = 0; 
    n = read(newsockfd, &dataSize, sizeof(int));
    if (n < 0) {
        std::cerr << "Error reading from socket" << std::endl;
        exit(0);
    }
    int* dataPos = new int[dataSize];
    bzero(dataPos, dataSize * sizeof(int));
    n = read(newsockfd, dataPos, msgSize);
    if (n < 0) {
        std::cerr << "Error reading from socket" << std::endl;
        exit(0);
    }

    int headSize = 0;
    n = 0; 
    n = read(newsockfd, &headSize, sizeof(int));
    if (n < 0) {
        std::cerr << "Error reading from socket" << std::endl;
        exit(0);
    }
    int* headPos = new int[headSize];
    bzero(headPos, headSize * sizeof(int));
    n = read(newsockfd, headPos, msgSize);
    if (n < 0) {
        std::cerr << "Error reading from socket" << std::endl;
        exit(0);
    }
    
    matrixEncoder(ranges, row, dataPos, headPos);
    
    n = 0;
    n = write(newsockfd, &rowSize, sizeof(int));
    if (n < 0) {
        std::cerr << "Error writing to socket" << std::endl;
        exit(0);
    }
    n = write(newsockfd, row, msgSize);
    if (n < 0) {
        std::cerr << "Error writing to socket" << std::endl;
        exit(0);
    } 
    
    close(newsockfd);
    exit(0);
}

void fireman(int) {
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

int main(int argc, char *argv[]) {
    if (argc != 2)  {
        std::cerr << "Port not provided" << std::endl;
        exit(0);
    }
    int sockfd, newsockfd, portno, clilen;
    struct sockaddr_in serv_addr, cli_addr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        std::cerr << "Error opening socket" << std::endl;
        exit(0);
    }

    // Populate the sockaddr_in structure
    bzero((char *)&serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    // Bind the socket with the sockaddr_in structure
    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Error binding" << std::endl;
        exit(0);
    }

    // Set the max number of concurrent connections
    listen(sockfd, 5);
    clilen = sizeof(cli_addr);
    signal(SIGCHLD, fireman);

    while (true) {
        newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, (socklen_t *)&clilen);
        if (fork() == 0) {
            if (newsockfd < 0) {
                std::cerr << "Error accepting new connections" << std::endl;
                exit(0);
            }
            clientInteraction(newsockfd);
        }
    }

    close(sockfd);
    return 0;
}