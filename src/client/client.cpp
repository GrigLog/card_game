#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h> //struct sockaddr_in

#include "common.h"

using namespace std; //I don't care

int main(int argc, char** argv) {
    cout << "Server is starting..." << endl;
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Failed to create a socket");
        exit(1);
    }
    in_addr_t server_address = INADDR_LOOPBACK; //localhost for now
    sockaddr_in address{AF_INET, htons(PORT), htonl(server_address)};
    if (connect(sockfd, reinterpret_cast<sockaddr*>(&address), sizeof(address)) == -1) {
        perror(format("Failed to connect to {}:{}", server_address, PORT).c_str());
        exit(1);
    }
    cout << "Connected to game server." << endl;    
}