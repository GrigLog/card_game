#include "game_server.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <stdexcept>
#include <thread>
#include <chrono>

GameServer::GameServer() 
    : acceptHandler(newPlayerPipe.getWriteFd()), 
        playerManager(newPlayerPipe.getReadFd()){
    std::cout << "Game server started. Type anything to stop it." << std::endl;
    std::string input;
    std::cin >> input;
}

GameServer::~GameServer() {
    std::cout << "Stopping game server..." << std::endl;
}

