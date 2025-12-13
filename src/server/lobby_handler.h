#pragma once

#include "room_manager.h"
#include "../common/message.h"
#include <queue>
#include <mutex>
#include <thread>
#include <atomic>
#include <unordered_map>
#include <poll.h>

// Обработчик лобби - работает в отдельном потоке
// Управляет игроками, которые еще не в комнатах
class LobbyHandler {
public:
    LobbyHandler(RoomManager& roomManager);
    ~LobbyHandler();
    
    // Запустить поток лобби
    void start();
    
    // Остановить поток лобби
    void stop();
    
    // Добавить нового игрока (вызывается из AcceptHandler)
    void addPlayer(int socketFd);

private:
    RoomManager& roomManager;
    
    // Игроки в лобби: playerId -> socketFd
    std::unordered_map<uint32_t, int> players;
    std::mutex playersMutex;
    
    // Очередь новых сокетов от AcceptHandler
    std::queue<int> newSockets;
    std::mutex newSocketsMutex;
    
    // Счетчик ID игроков
    std::atomic<uint32_t> nextPlayerId;
    
    // Поток лобби
    std::thread lobbyThread;
    std::atomic<bool> running;
    
    // Основной цикл потока лобби
    void run();
    
    // Обработка команды от игрока
    void handleCommand(uint32_t playerId, const std::string& command);
    
    // Отправить ответ игроку
    void sendToPlayer(uint32_t playerId, const std::string& response);
};
