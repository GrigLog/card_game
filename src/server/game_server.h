#pragma once

#include "accept_handler.h"
#include "lobby_handler.h"
#include "room_manager.h"
#include "../common/common.h"
#include <memory>

// RAII класс с основной логикой сервера
class GameServer {
    //int listenSocketFd;
    std::unique_ptr<AcceptHandler> acceptHandler;
    std::unique_ptr<RoomManager> roomManager;
    std::unique_ptr<LobbyHandler> lobbyHandler;
    std::atomic<bool> running;

    int lobbyWakeupFd[2];

public:
    GameServer();
    ~GameServer();
    
    // Сервер запускается в текущем потоке (возврат управления будет только после остановки сервера)
    void start();
    
    // Остановить сервер
    void stop();
    
    // Проверить, запущен ли сервер
    bool isRunning() const { return running; }

private:    
    // Создать слушающий сокет
    int createListenSocket();
};
