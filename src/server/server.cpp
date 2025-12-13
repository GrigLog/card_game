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
    sockaddr_in address{AF_INET, htons(PORT), htonl(INADDR_LOOPBACK)};
    if (bind(sockfd, reinterpret_cast<sockaddr*>(&address), sizeof(address)) == -1) {
        perror(format("Failed to bind socket to port {}", PORT).c_str());
        exit(1);
    }
    if (listen(sockfd, 1000) == -1) {
        perror(format("Socket on port {} could not start listening", PORT).c_str());
        exit(1);
    }
    while (true) {
        sockaddr_in peer_address;
        socklen_t peer_address_size = sizeof(peer_address);
        int active_sockfd = accept(sockfd, reinterpret_cast<sockaddr*>(&peer_address), &peer_address_size);
        cout << "Accepted connection from " << ntohl(peer_address.sin_addr.s_addr) 
            << ":" << ntohs(peer_address.sin_port) << endl;
    }
    
}