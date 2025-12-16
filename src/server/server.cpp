#include <atomic>
#include <chrono>
#include <csignal>
#include <iostream>
#include <thread>

#include "game_server.h"

volatile bool running{true};

void signalHandler(int signal) {
    if (signal == SIGINT || signal == SIGTERM) {
        running = false;
    }
}

int main(int argc, char** argv) {
    std::cout << "Server is starting..." << std::endl;

    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    try {
        TGameServer server;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    std::cout << "Game server stopped." << std::endl;

    return 0;
}