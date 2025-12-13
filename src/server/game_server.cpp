#include "game_server.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <stdexcept>
#include <thread>
#include <chrono>

//Used to configure the server. It is not started here.
GameServer::GameServer() : running(false) {
    // if (listenSocketFd < 0) {
    //     throw std::runtime_error("Failed to create listen socket");
    // }
    roomManager = std::make_unique<RoomManager>();
    lobbyHandler = std::make_unique<LobbyHandler>(*roomManager);
    acceptHandler = std::make_unique<AcceptHandler>();
}

void GameServer::start() {
    if (running) 
        return;
    running = true;

    if (pipe(lobbyWakeupFd) == -1) {
        perror("pipe failed");
        exit(1);
    }

    acceptHandler->start(lobbyWakeupFd[1]);
    lobbyHandler->start(lobbyWakeupFd[0]);
    
    
    std::cout << "Game server started. Type anything to stop it." << std::endl;
    std::string input;
    std::cin >> input;
}

GameServer::~GameServer() {
    stop();
}

void GameServer::stop() {
    if (!running)
        return;
    running = false;
    std::cout << "Stopping game server..." << std::endl;
    // if (listenSocketFd >= 0) {
    //     close(listenSocketFd);
    // }
    std::cout << "Game server stopped." << std::endl;
}

