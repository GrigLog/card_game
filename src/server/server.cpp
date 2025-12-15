#include <atomic>
#include <chrono>
#include <csignal>
#include <iostream>
#include <thread>

#include "game_server.h"

std::atomic<bool> g_running{true};

void signalHandler(int signal) {
    if (signal == SIGINT || signal == SIGTERM) {
        g_running = false;
    }
}

int main(int argc, char** argv) {
    std::cout << "Server is starting..." << std::endl;

    // Устанавливаем обработчик сигналов
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    try {
        GameServer server;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    std::cout << "Game server stopped." << std::endl;

    return 0;
}