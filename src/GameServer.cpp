#include "card_game/GameServer.h"
#include <algorithm>

namespace card_game {

GameServer::GameServer() : running_(false) {}

GameServer::~GameServer() {
    stop();
}

void GameServer::start() {
    if (running_) {
        return;
    }
    
    running_ = true;
    workerThread_ = std::thread(&GameServer::workerLoop, this);
}

void GameServer::stop() {
    if (!running_) {
        return;
    }
    
    running_ = false;
    queueCondition_.notify_all();
    
    if (workerThread_.joinable()) {
        workerThread_.join();
    }
}

void GameServer::addPlayer(std::shared_ptr<IPlayer> player) {
    std::lock_guard<std::mutex> lock(playersMutex_);
    players_.push_back(std::move(player));
}

void GameServer::createSession(const std::vector<int>& playerIds) {
    std::lock_guard<std::mutex> lockPlayers(playersMutex_);
    std::lock_guard<std::mutex> lockSessions(sessionsMutex_);
    
    std::vector<std::shared_ptr<IPlayer>> sessionPlayers;
    
    for (int id : playerIds) {
        auto it = std::find_if(players_.begin(), players_.end(),
            [id](const auto& p) { return p->getId() == id; });
        
        if (it != players_.end()) {
            sessionPlayers.push_back(*it);
        }
    }
    
    if (sessionPlayers.size() >= 2) {
        auto session = std::make_unique<GameSession>(std::move(sessionPlayers));
        session->start();
        sessions_.push_back(std::move(session));
    }
}

void GameServer::handlePlayerAction(int playerId, std::unique_ptr<Action> action) {
    std::lock_guard<std::mutex> lock(queueMutex_);
    actionQueue_.push({playerId, std::move(action)});
    queueCondition_.notify_one();
}

void GameServer::workerLoop() {
    while (running_) {
        std::unique_lock<std::mutex> lock(queueMutex_);
        
        queueCondition_.wait(lock, [this] {
            return !actionQueue_.empty() || !running_;
        });
        
        if (!running_ && actionQueue_.empty()) {
            break;
        }
        
        if (!actionQueue_.empty()) {
            auto [playerId, action] = std::move(actionQueue_.front());
            actionQueue_.pop();
            lock.unlock();
            
            // Обрабатываем действие в первой доступной сессии
            std::lock_guard<std::mutex> sessionLock(sessionsMutex_);
            for (auto& session : sessions_) {
                if (!session->isGameOver()) {
                    session->processAction(playerId, std::move(action));
                    break;
                }
            }
        }
    }
}

} // namespace card_game

