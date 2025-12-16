#include "game_server.h"

#include <chrono>
#include <fcntl.h>
#include <iostream>
#include <netinet/in.h>
#include <stdexcept>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>

TGameServer::TGameServer()
    : AcceptHandler(NewPlayerPipe.getWriteFd())
    , PlayerManager(NewPlayerPipe.getReadFd())
{
    std::cout << "Game server started. Type anything to stop it." << std::endl;
    std::string input;
    std::cin >> input;
}

TGameServer::~TGameServer() {
    std::cout << "Stopping game server..." << std::endl;
}
