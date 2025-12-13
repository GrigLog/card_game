#pragma once

#include "../common/common.h"
#include <queue>
#include <mutex>
#include <thread>
#include <atomic>

// Обработчик приема новых соединений
// Работает в отдельном потоке
class AcceptHandler {
    int listenSocketFd;
    std::queue<int> newSockets;
    std::mutex socketsMutex;
    std::thread acceptThread;
    std::atomic<bool> running;

public:
    AcceptHandler(int listenSocketFd);
    ~AcceptHandler();
    
    // Запустить поток приема
    void start();
    
    // Остановить поток приема
    void stop();
    
    // Получить новый сокет (вызывается из GameServer)
    // Возвращает -1 если нет новых соединений
    int getNewSocket();

private:
    // Основной цикл потока приема
    void run();
};
