#include "game_server.h"
#include <iostream>
#include <csignal>
#include <atomic>
#include <thread>
#include <chrono>

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
        server.start();
        // Все, что делает этот поток - ждет сигнала остановки
        //while (g_running && server.isRunning()) {
        //    std::this_thread::sleep_for(std::chrono::milliseconds(100));
        //}
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}