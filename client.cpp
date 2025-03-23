/*
Ali Husain
Dr. Rincon
COSC 3360: Programming Assignment 2
22 Mar 2025
*/

/*
struct lines { // defining lines struct from data given by input
    std::vector<std::pair<char, std::vector<std::pair<int, int> > > >& ranges;
    std::vector<int> headPos;
    std::vector<int>& dataPos;
    int index;
    char** outputMatrix; // make changes to matrix when done with figuring out character placement
    char *serverIP
    char *portNum
};
Struct vectors will have to be changed into arrays
haed pos and data pos will be easily changed to arrays
ranges will be harder, maybe struct in struct?
output the matrix using strings and prints no more matrix maybe
where does lines come into play?

void * decoder(void *void_ptr); is our thread function 
*/
#include <unistd.h>
#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <strings.h>

int main(int argc, char *argv[]) {
    
    return 0;
}