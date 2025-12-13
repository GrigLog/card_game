#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <mutex>

// Минимальная игровая комната - только хранит игроков
class GameRoom {
public:
    GameRoom(const std::string& name, uint32_t ownerId, size_t maxPlayers);
    
    std::string getName() const { return name; }
    uint32_t getOwnerId() const { return ownerId; }
    
    // Добавить игрока (по сокету)
    bool addPlayer(uint32_t playerId, int socketFd);
    
    // Проверить, полна ли комната
    bool isFull() const;
    
    // Количество игроков
    size_t playerCount() const;
    
    // Получить список ID игроков
    std::vector<uint32_t> getPlayerIds() const;

private:
    std::string name;
    uint32_t ownerId;
    size_t maxPlayers;
    
    struct PlayerInfo {
        uint32_t id;
        int socketFd;
    };
    std::vector<PlayerInfo> players;
    mutable std::mutex playersMutex;
};
