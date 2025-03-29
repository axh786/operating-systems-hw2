/*
Ali Husain
Dr. Rincon
COSC 3360: Programming Assignment 2
22 Mar 2025
*/
// The socket related code is provided by Dr. Rincons examples in canvas
#include <unistd.h>
#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sstream>
#include <vector>
#include <string.h>
#include <sys/wait.h>

void matrixEncoder(char* range, char* row, int* dataPos, int* headPos) { //algorithm from PA1 
    std::vector<std::pair<char, std::vector<std::pair<int, int> > > > ranges;
    std::istringstream iss(range); // turn ranges into a vector pair to be more manageable
    std::string token;
    while (std::getline(iss, token, ',')) {
        char id = token[0];
        std::vector<std::pair<int, int> > pairs;
        std::istringstream subIss(token.substr(2));
        int a, b;
        while (subIss >> a >> b) {
            pairs.emplace_back(a, b);
        }
        ranges.emplace_back(id, pairs);
    }

    for (int i = headPos[0]; i < headPos[1]; i++) { // loops through the range
         int data = dataPos[i];
         for (int j = 0; j < ranges.size(); j++) {
             char character = ranges[j].first;
             const std::vector<std::pair<int, int> >& inner_ranges = ranges[j].second;
             for (int k = 0; k < inner_ranges.size(); k++) {
                 int start = inner_ranges[k].first; // gets the ranges from the nested vector pair
                 int end = inner_ranges[k].second;
                 if (data >= start && data <= end) { // checks to see if data falls in range if true adds char to the matrix
                    row[data] = character;
                    break;
                 }
                 
             }
         }
    }

}

void clientInteraction(int newsockfd) {
    // function for interacting with client, reading and writing i/o, provided from Dr. Rincon
    int n, msgSize = 0;
    n = read(newsockfd, &msgSize, sizeof(int)); // reads in  ranges of the chars
    if (n < 0) {
        std::cerr << "Error reading from socket1" << std::endl;
        exit(0);
    }
    char* ranges = new char[msgSize+1];
    bzero(ranges, msgSize + 1);
    n = read(newsockfd, ranges, msgSize);
    if (n < 0) {
        std::cerr << "Error reading from socket2" << std::endl;
        exit(0);
    }

    n = 0;
    int rowSize = 0; // reads in matrix row that will be decoded
    n = read(newsockfd, &rowSize, sizeof(int));
    if (n < 0) {
        std::cerr << "Error reading from socket3" << std::endl;
        exit(0);
    }
    char* row = new char[rowSize+1];
    bzero(row, rowSize + 1);
    n = read(newsockfd, row, rowSize);
    if (n < 0) {
        std::cerr << "Error reading from socket4" << std::endl;
        exit(0);
    }

    int dataSize = 0; // reads in dataPos arr that contains all of the positions in the matrix
    n = 0; 
    n = read(newsockfd, &dataSize, sizeof(int));
    if (n < 0) {
        std::cerr << "Error reading from socket5" << std::endl;
        exit(0);
    }
    int* dataPos = new int[dataSize];
    bzero(dataPos, dataSize * sizeof(int));
    n = read(newsockfd, dataPos, dataSize * sizeof(int));
    if (n < 0) {
        std::cerr << "Error reading from socket6" << std::endl;
        exit(0);
    }

    int headSize = 0; // reads in headPos arr that contains all the head positions, start and stop
    n = 0; 
    n = read(newsockfd, &headSize, sizeof(int));
    if (n < 0) {
        std::cerr << "Error reading from socket7" << std::endl;
        exit(0);
    }
    int* headPos = new int[headSize];
    bzero(headPos, headSize * sizeof(int));
    n = read(newsockfd, headPos, headSize * sizeof(int));
    if (n < 0) {
        std::cerr << "Error reading from socket8" << std::endl;
        exit(0);
    }
    
    matrixEncoder(ranges, row, dataPos, headPos); // function call that will edit row and take in all the information read
    
    n = 0;
    rowSize = strlen(row); // send the row back to the server
    n = write(newsockfd, &rowSize, sizeof(int));
    if (n < 0) {
        std::cerr << "Error writing to socket" << std::endl;
        exit(0);
    }
    n = write(newsockfd, row, rowSize);
    if (n < 0) {
        std::cerr << "Error writing to socket" << std::endl;
        exit(0);
    } 
    
    delete[] ranges;
    delete[] row;
    delete[] dataPos;
    delete[] headPos;


    close(newsockfd); // close the socket
    exit(0);
}

void fireman(int) { // firemen loop to clear up zombie processes
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

int main(int argc, char *argv[]) {
    // Socket related code provieded from Dr. Rincon's examples in canvas
    if (argc != 2)  { // ensure that 2 arguments are ran
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
    
    int yes=1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) {
        perror("setsockopt");
        exit(1);
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
        if (fork() == 0) { // fork established to create child processes for the given input from client
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