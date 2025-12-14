#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <mutex>
#include <queue>
#include <unordered_set>
#include <unordered_map>
#include <variant>
#include "command.h"

#include "actor/actor.h"
#include "actor/bot.h"

// Игровая комната - принимает команды игроков и как-то на них реагирует.
// Не имеет своего потока (кроме, может, таймаута?)
class GameRoom {
public:
    const std::string name;
    const unsigned ownerId;
    const size_t maxPlayers;

    unsigned nextBotId = 0;
    bool bStarted = false;
    
    std::vector<std::unique_ptr<IActor>> actors;

    //NOTE: should be refreshed from time to time to remove null pointers
    inline static std::unordered_map<std::string, std::weak_ptr<GameRoom>> allRooms;

public:
    static std::shared_ptr<GameRoom> make(const std::string& name, uint32_t ownerId, size_t maxPlayers) {
        auto res = std::make_shared<GameRoom>(name, ownerId, maxPlayers);
        allRooms[name] = std::weak_ptr(res);
        return res;
    }
    //I can't declare it as private but please don't use it
    GameRoom(const std::string& name, uint32_t ownerId, size_t maxPlayers);

    static std::vector<const GameRoom*> getAllRooms();
    
    bool addPlayer(unsigned playerId);

    bool addBot(std::unique_ptr<IBotStrategy>&& strategy);
    
    // Проверить, полна ли комната
    bool isFull() const;

    void notifyPlayerLeft(unsigned playerId);

    std::string handleCommand(unsigned playerId, SomeCommand cmd);
    std::string executeRoomCommand(unsigned playerId, RoomCommand cmd);

    ~GameRoom() {
        allRooms.erase(name);
        std::cout << "Room '" << name << "' was deleted." << std::endl;
    }
private:
    
};
