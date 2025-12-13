#include "accept_handler.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>
#include <errno.h>

AcceptHandler::AcceptHandler(int listenSocketFd)
    : listenSocketFd(listenSocketFd), running(false) {
}

AcceptHandler::~AcceptHandler() {
    stop();
}

void AcceptHandler::start() {
    if (running) {
        return;
    }
    running = true;
    acceptThread = std::thread(&AcceptHandler::run, this);
}

void AcceptHandler::stop() {
    if (!running) {
        return;
    }
    running = false;
    if (acceptThread.joinable()) {
        acceptThread.join();
    }
}

int AcceptHandler::getNewSocket() {
    std::lock_guard<std::mutex> lock(socketsMutex);
    if (newSockets.empty()) {
        return -1;
    }
    int socketFd = newSockets.front();
    newSockets.pop();
    return socketFd;
}

void AcceptHandler::run() {
    while (running) {
        sockaddr_in peer_address;
        socklen_t peer_address_size = sizeof(peer_address);
        
        int active_sockfd = accept(listenSocketFd, reinterpret_cast<sockaddr*>(&peer_address), &peer_address_size);
        
        if (active_sockfd < 0) {
            if (errno == EINTR) {
                continue;
            }
            if (running) {
                perror("Accept failed");
            }
            break;
        }
        
        std::cout << "Accepted connection from " 
                  << ntohl(peer_address.sin_addr.s_addr) 
                  << ":" << ntohs(peer_address.sin_port) << std::endl;
        
        // Добавляем сокет в очередь
        {
            std::lock_guard<std::mutex> lock(socketsMutex);
            newSockets.push(active_sockfd);
        }
    }
}
