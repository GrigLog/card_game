#pragma once

#include "GameSession.h"
#include "IPlayer.h"
#include <vector>
#include <memory>
#include <thread>
#include <atomic>
#include <mutex>
#include <queue>

namespace card_game {

// Игровой сервер (упрощенная версия без реальных сокетов для демонстрации)
class GameServer {
public:
    GameServer();
    ~GameServer();
    
    void start();
    void stop();
    
    // Добавление игроков
    void addPlayer(std::shared_ptr<IPlayer> player);
    
    // Создание новой игровой сессии
    void createSession(const std::vector<int>& playerIds);
    
    // Обработка действия от игрока
    void handlePlayerAction(int playerId, std::unique_ptr<Action> action);
    
    bool isRunning() const { return running_; }
    
private:
    std::vector<std::shared_ptr<IPlayer>> players_;
    std::vector<std::unique_ptr<GameSession>> sessions_;
    std::atomic<bool> running_;
    std::mutex playersMutex_;
    std::mutex sessionsMutex_;
    
    // Асинхронная обработка действий
    std::thread workerThread_;
    std::queue<std::pair<int, std::unique_ptr<Action>>> actionQueue_;
    std::mutex queueMutex_;
    std::condition_variable queueCondition_;
    
    void workerLoop();
};

} // namespace card_game

