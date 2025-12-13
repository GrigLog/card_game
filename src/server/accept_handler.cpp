#include "accept_handler.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>
#include <errno.h>

AcceptHandler::AcceptHandler() : running(false) {
}

AcceptHandler::~AcceptHandler() {
    stop();
}

void AcceptHandler::start(int wakeupWriteFd) {
    if (running) 
        return;
    running = true;
    listenSocketFd = createListenSocket();
    //std::cout << "listening socket = " << listenSocketFd << std::endl;
    wakeupWriteFd = wakeupWriteFd;
    runThread = std::thread(&AcceptHandler::run, this);
}

void AcceptHandler::stop() {
    if (!running) {
        return;
    }
    running = false;
    std::cout << "Stopping AcceptHandler..." << std::endl;
    if (wakeupWriteFd >= 0) {
        close(wakeupWriteFd);
    }
    if (listenSocketFd >= 0) {
        //std::cout << "closed listening socket " << listenSocketFd << std::endl;
        shutdown(listenSocketFd, SHUT_RDWR);  //NOTE: only shutdown() guarantees escaping blocking functions such as accept()
        close(listenSocketFd);
    }
    if (runThread.joinable()) {
        runThread.join();
    }
    std::cout << "AcceptHandler stopped." << std::endl;
}

// int AcceptHandler::getNewSocket() {
//     return newSockets.pop();
// }

void AcceptHandler::run() {
    while (running) {
        sockaddr_in peer_address;
        socklen_t peer_address_size = sizeof(peer_address);
        //std::cout << "enter accept" << std::endl;
        int active_sockfd = accept(listenSocketFd, reinterpret_cast<sockaddr*>(&peer_address), &peer_address_size);
        //std::cout << "exit accept" << std::endl;
        if (active_sockfd < 0) {
            if (running) {
                perror("Accept failed");
            }
            break;
        }
        
        std::cout << "Accepted connection from " 
                  << ntohl(peer_address.sin_addr.s_addr) 
                  << ":" << ntohs(peer_address.sin_port) << std::endl;
        
        // Добавляем сокет в очередь
        write(wakeupWriteFd, &active_sockfd, sizeof(active_sockfd));
        //newSockets.push(active_sockfd);
    }
}

int AcceptHandler::createListenSocket() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Failed to create socket");
        return -1;
    }
    
    // Устанавливаем опцию SO_REUSEADDR
    int opt = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt failed");
        close(sockfd);
        return -1;
    }
    
    sockaddr_in address{AF_INET, htons(PORT), INADDR_ANY};  //INADDR_ANY = 0 = Слушаем на всех интерфейсах
    if (bind(sockfd, reinterpret_cast<sockaddr*>(&address), sizeof(address)) == -1) {
        perror(std::format("Failed to bind socket to port {}", PORT).c_str());
        close(sockfd);
        return -1;
    }
    
    if (listen(sockfd, 1000) == -1) {
        perror(std::format("Socket on port {} could not start listening", PORT).c_str());
        close(sockfd);
        return -1;
    }
    
    std::cout << "Server listening on port " << PORT << std::endl;
    return sockfd;
}

