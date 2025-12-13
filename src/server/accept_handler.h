#pragma once

#include "../common/common.h"
#include <queue>
#include <mutex>
#include <thread>
#include <atomic>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>

// Обработчик приема новых соединений
class AcceptHandler {
private:
    //BlockingQueue<int> newSockets;
    int listenSocketFd = -1;
    int newPlayerPipeFd = -1;

    volatile bool running = true;
    std::thread runThread;

public:
    AcceptHandler(int newPlayerPipeFd) 
    : newPlayerPipeFd(newPlayerPipeFd) {
        listenSocketFd = createListenSocket();
        //std::cout << "listening socket = " << listenSocketFd << std::endl;
        runThread = std::thread(&AcceptHandler::run, this);
    }

    ~AcceptHandler() {
        running = false;
        std::cout << "AcceptHandler is stopping..." << std::endl;
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

private:
    // Основной цикл потока приема
    void run();

    int createListenSocket();
};
