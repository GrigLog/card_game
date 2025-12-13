#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <mutex>

// Игровая комната - работает в отдельном потоке, принимает команды игроков и как-то на них реагирует
class GameRoom {
public:
    const std::string name;
    const unsigned ownerId;
    const size_t maxPlayers;
    
    std::vector<unsigned> players;
    mutable std::mutex playersMutex;

public:
    GameRoom(const std::string& name, uint32_t ownerId, size_t maxPlayers);
    
    // Добавить игрока (по сокету)
    bool addPlayer(unsigned playerId);
    
    // Проверить, полна ли комната
    bool isFull() const;
};
