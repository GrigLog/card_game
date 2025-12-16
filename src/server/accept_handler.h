#pragma once

#include <atomic>
#include <errno.h>
#include <iostream>
#include <mutex>
#include <netinet/in.h>
#include <queue>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>

#include "../common/common.h"

// Обработчик приема новых соединений
class TAcceptHandler {
private:
    // BlockingQueue<int> newSockets;
    int listenSocketFd = -1;
    int newPlayerPipeFd = -1;

    volatile bool running = true;
    std::thread runThread;

public:
    TAcceptHandler(int newPlayerPipeFd)
        : newPlayerPipeFd(newPlayerPipeFd) {
        listenSocketFd = createListenSocket();
        // std::cout << "listening socket = " << listenSocketFd << std::endl;
        runThread = std::thread(&TAcceptHandler::run, this);
    }

    ~TAcceptHandler() {
        running = false;
        std::cout << "AcceptHandler is stopping..." << std::endl;
        if (listenSocketFd >= 0) {
            // std::cout << "closed listening socket " << listenSocketFd << std::endl;
            shutdown(listenSocketFd,
                     SHUT_RDWR); // NOTE: only shutdown() guarantees escaping
                                 // blocking functions such as accept()
            close(listenSocketFd);
        }
        if (runThread.joinable()) {
            runThread.join();
        }
        std::cout << "AcceptHandler stopped." << std::endl;
    }

private:
    // Основной цикл потока приема
    void run();

    int createListenSocket();
};
