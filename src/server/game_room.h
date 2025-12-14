#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <mutex>
#include <queue>

#include "actor/actor.h"
#include "actor/bot.h"
#include "commands/game/game_command.h"

// Игровая комната - принимает команды игроков и как-то на них реагирует.
// Не имеет своего потока (кроме, может, таймаута?)
class GameRoom {
public:
    const std::string name;
    const unsigned ownerId;
    const size_t maxPlayers;

    unsigned nextBotId;
    bool bStarted;
    
    std::vector<std::unique_ptr<IActor>> actors;

public:
    GameRoom(const std::string& name, uint32_t ownerId, size_t maxPlayers);
    
    // Добавить игрока (по сокету)
    bool addPlayer(unsigned playerId);

    bool addBot(std::unique_ptr<IBotStrategy>&& strategy);
    
    // Проверить, полна ли комната
    bool isFull() const;
};
