#include "game_server.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <stdexcept>
#include <thread>
#include <chrono>

using std::format;

GameServer::GameServer() : listenSocketFd(-1), running(false) {
    listenSocketFd = createListenSocket();
    if (listenSocketFd < 0) {
        throw std::runtime_error("Failed to create listen socket");
    }
    
    roomManager = std::make_unique<RoomManager>();
    lobbyHandler = std::make_unique<LobbyHandler>(*roomManager);
    acceptHandler = std::make_unique<AcceptHandler>(listenSocketFd);
}

GameServer::~GameServer() {
    stop();
    if (listenSocketFd >= 0) {
        close(listenSocketFd);
    }
}

int GameServer::createListenSocket() {
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
    
    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_port = htons(PORT);
    address.sin_addr.s_addr = htonl(INADDR_ANY); // Слушаем на всех интерфейсах
    
    if (bind(sockfd, reinterpret_cast<sockaddr*>(&address), sizeof(address)) == -1) {
        perror(format("Failed to bind socket to port {}", PORT).c_str());
        close(sockfd);
        return -1;
    }
    
    if (listen(sockfd, 1000) == -1) {
        perror(format("Socket on port {} could not start listening", PORT).c_str());
        close(sockfd);
        return -1;
    }
    
    std::cout << "Server listening on port " << PORT << std::endl;
    return sockfd;
}

void GameServer::start() {
    if (running) {
        return;
    }
    
    running = true;
    acceptHandler->start();
    lobbyHandler->start();
    
    // Связываем AcceptHandler с LobbyHandler
    std::thread connector([this]() {
        while (running) {
            int socketFd = acceptHandler->getNewSocket();
            if (socketFd >= 0) {
                lobbyHandler->addPlayer(socketFd);
            } else {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        }
    });
    connector.detach();
    
    std::cout << "Game server started" << std::endl;
}

void GameServer::stop() {
    if (!running) {
        return;
    }
    
    running = false;
    
    if (acceptHandler) {
        acceptHandler->stop();
    }
    
    if (lobbyHandler) {
        lobbyHandler->stop();
    }
    
    std::cout << "Game server stopped" << std::endl;
}
