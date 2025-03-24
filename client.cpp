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
#include <sstream>
#include <pthread.h>

struct row { // defining row struct from data given by input so each row will have the ranges to search thru, which head pos they are needed and then loop thru data pos
    int cols;
    char* ranges;
    int headPos[2];
    int* dataPos;
    int dataSize;
    int index;
    char** outputMatrix; // make changes to matrix when done with figuring out character placement
    
    char* serverIP;
    char* portno;
};

void createSocket(char* portNoRaw, char* serverIP, int& sockfd) {
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

void sendMatrixData(char* row, int cols, int sockfd){
    int n;
    n = write(sockfd,&cols,sizeof(int));
    if (n < 0) {
        std::cerr << "ERROR writing to socket" << std::endl;
        exit(0);
    }
    n = write(sockfd,row,cols);
    if (n < 0) {
        std::cerr << "ERROR writing to socket" << std::endl;
        exit(0);
    }
}

void sendIntarrData(int* arr, int arrSize, int sockfd) {
    int n;
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
    std::strcpy(outputMatrix[index], buffer);
}

void * decoder(void *void_ptr) {  // thread function
    int sockfd;
    struct row* ptr = (row*) void_ptr;
    createSocket(ptr->portno, ptr->serverIP, sockfd); // opening a socket
    sendStringData(ptr->ranges, sockfd); // send ranges over to server
    sendMatrixData(ptr->outputMatrix[ptr->index], ptr->cols, sockfd); // send the row that will be edited to the server
    sendIntarrData(ptr->dataPos, ptr->dataSize, sockfd); // sending datapos
    sendIntarrData(ptr->headPos, 2, sockfd); // sending headpos
    readData(ptr->index, ptr->outputMatrix, sockfd); // read data from the server

    close (sockfd);
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
       std::cerr << "usage " << argv[0] << " hostname port" << std::endl;
       exit(0);
    }
    char* serverIP = argv[1];
    char* portno = argv[2];
    int cols, rows; // reading matrix size from first line
    std::cin >> cols >> rows;
    std::cin.ignore();
    char** outputMatrix = new char*[rows];
    
    for (int r = 0; r < rows; ++r) { // initalizing 2d array
        outputMatrix[r] = new char[cols];
        std::fill(outputMatrix[r], outputMatrix[r] + cols, ' '); // fill 2d array with spaces before decoding
    }
    std::string ranges; // reading second line to get the ranges, stored as a string
    std::getline(std::cin, ranges);

    std::string headLine; // reading third line to get the headPos arr, stored as an integer array
    std::getline(std::cin, headLine);
    std::stringstream ssCountHead(headLine);
    int value, headSize = 0;
    while (ssCountHead >> value) {++headSize;}
    int* headPosition = new int[headSize];
    std::stringstream ssHead(headLine);
    for (int i = 0; i < headSize; ++i) {ssHead >> headPosition[i];}

    std::string dataLine; // reading fourth line to get the dataPos arr, stored as an integer array
    std::getline(std::cin, dataLine);
    std::stringstream ssCountData(dataLine);
    int dataSize = 0;
    while (ssCountData >> value) {++dataSize;}
    int* dataPosition = new int[dataSize];
    std::stringstream ssData(dataLine);
    for (int i = 0; i < dataSize; ++i) {ssData >> dataPosition[i];}

    // threading and outputting will both be done in main
    std::vector<row> lines;
    lines.reserve(rows);
     for (int i = 0; i < rows; i++) {
        row line;
        line.cols = cols;
        line.ranges = new char[ranges.size()+1]; // put ranges in each member
        std::strcpy(line.ranges, ranges.c_str());
        line.headPos[0] = headPosition[i]; // filling in head pos
        if (i == rows - 1) {line.headPos[1]= dataSize;} // if that checks if last section, if it is set end to size of data pos
        else {line.headPos[1] = headPosition[i+1];}
        line.dataPos = dataPosition; // set to dataPosition, index, outputmatrix and serverip + portno
        line.dataSize = dataSize;
        line.index = i;
        line.outputMatrix = outputMatrix;
        line.serverIP = serverIP;
        line.portno = portno;

        lines.push_back(line);
    }
    
    pthread_t *tid = new pthread_t[rows]; // dynamic array of pthread_t of size of the image height (rows), based off of Dr. Rincon's threading practices
    for (int i = 0; i < rows; i++) { // for loop that iterates through the lines in the image (amount of rows)
        if(pthread_create(&tid[i],nullptr,decoder,(void *) &lines.at(i))!= 0) {
			std::cerr << "Error creating thread" << std::endl;
			return 1;
		}
    }

    for (int j = 0; j < rows; j++) { // Waits for other threads to finish then call pthread_join 
        pthread_join(tid[j],nullptr);
    }

    for (int r = 0; r < rows; r++) { // printing out the matrix 
        for (int c = 0; c < cols; c++) {
            std::cout << outputMatrix[r][c];
        }
        std::cout << std::endl;
    }



    for (int r = 0; r < rows; ++r) { // deallocating memory for matrix
        delete[] outputMatrix[r];
    }
    delete[] outputMatrix;
    
    for (int i = 0; i < lines.size(); ++i) { // deallocating memory for lines ranges
        delete[] lines[i].ranges;
    }
    
    delete[] tid;
    delete[] headPosition;
    delete[] dataPosition;

    return 0;
}