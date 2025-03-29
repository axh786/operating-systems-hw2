/*
Ali Husain
Dr. Rincon
COSC 3360: Programming Assignment 2
22 Mar 2025
*/

/*
NOTE FOR GRADER PLEASE READ:
Starting on line 213, the backlog via the template code was set to 5. In MacOS setting it to 5 hard
caps it to 1-5 connections being queued. Linux, despite it being set to 5, would silently increase
it to allow for more space in the queue and no dropped connections, when needed. Thats why, when I 
ran it on moodle prior it worked every time (my code was submitted multiple times identically)
while on macOS I was sometimes getting "ERROR reading from socket9" clientside regarding input1.
This was remedied by increasing it to 8, allowing input1 to work the every time. 

The odd part to all of this was the fact that input1 (UH) would be giving me these issues BUT
input2 (COSC 3360) was working everytime. The reason for this lies in how I threaded the
function. As per the instructions in PA1 & PA2, each line of the output is its own thread who opens their
socket to the server and do their decoding which is then recieved again by said thread. The reason 
why input1 wouldnt work was due to the 7 threads being created (26 (cols) by 7 LINES(rows)) would
overwhelm the queuebacklog of 5. So now the next question I had was why is input1 SOMETIMES working
but not all the time, moreso why is it not failing everytime? This all depends on scheduling from
the CPU and when/what order these connections come. If the server is even slightly slow, like with
forking, it will not be able to catchup to the 7 thread requirement thus being filled, dropping
connections and throwing the ERROR. In the lucky case where it worked, TCP Retransmission occurs
which retries dropped connections. This has varying degrees of success but can work and give the
scheduler just enough time to process all of the requests and for the sever to catch up, correctly 
outputting the UH char representation.  Setting it to 8 makes sure that the OS scheduling & server
has some breathing room and just enough time to process everything and have consistent output everytime.

The change to the backlog: listen(sockfd, 5); ->  listen(sockfd, 8);

NOTE THAT WITH listen() SET TO 5 AND WITHOUT THE IF STATEMENT ON LINE 193 THAT THE PROGRAM WAS WORKING 
ON MOODLE AND LINUX 100% OF THE TIME, PLEASE SEE THE PREVIOUS SUBMISSIONS. 
*/
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
// The socket related code is provided by Dr. Rincons examples in canvas
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
    
    int yes=1; // code provided from StackOverFlow regarding an error of brinding
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0) {
        perror("setsockopt");
        exit(1);
    }
    // specific to MacOS to enable multiple processes and threads to bind to the same port
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &yes, sizeof(yes)) < 0) {
        std::cerr << "Error setting SO_REUSEPORT" << std::endl;
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
    listen(sockfd, 8); // After a lot of testing and wondering why my code wasnt working on MacOS I figured out I had to increase the backlog
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