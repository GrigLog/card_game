#pragma once

#include "../common/common.h"
#include <queue>
#include <mutex>
#include <thread>
#include <atomic>

// Обработчик приема новых соединений
class AcceptHandler {
private:
    //BlockingQueue<int> newSockets;
    int listenSocketFd = -1;
    int wakeupWriteFd = -1;
    std::atomic<bool> running;

    std::thread runThread;

public:
    AcceptHandler();
    ~AcceptHandler();
    
    // Запустить прием входящих подключений в новом потоке.
    // Результаты записываются в очередь newSockets.
    void start(int wakeupWriteFd);
    
    // Остановить поток приема
    void stop();
    
    // Получить новый сокет (блокирующая операция)
    //int getNewSocket();

private:
    // Основной цикл потока приема
    void run();

    int createListenSocket();
};
