#pragma once

#include <cstdint>
#include <mutex>
#include <optional>
#include <queue>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>

#include "actor/actor.h"
#include "command.h"
#include "durak/durak.h"

// Игровая комната - принимает команды игроков и как-то на них реагирует.
// Не имеет своего потока
class TGameRoom {
public:
    const std::string Name;
    const unsigned OwnerId;
    const size_t MaxPlayers;

    unsigned NextBotId = 0;
    bool Started = false;
    std::optional<TDurakGame> GameOpt{};

    std::vector<std::unique_ptr<IActor>> Actors;
    std::unordered_map<unsigned, unsigned> PlayerIdToActorNum;

    // NOTE: should be refreshed from time to time to remove null pointers
    inline static std::unordered_map<std::string, std::weak_ptr<TGameRoom>>
        AllRooms;

public:
    // I can't declare it as private but please don't use it
    TGameRoom(const std::string& name, uint32_t ownerId, size_t maxPlayers);

    ~TGameRoom() {
        AllRooms.erase(Name);
        std::cout << "Room '" << Name << "' was deleted." << std::endl;
    }

    static std::shared_ptr<TGameRoom> make(const std::string& name,
                                          uint32_t ownerId, size_t maxPlayers) {
        auto res = std::make_shared<TGameRoom>(name, ownerId, maxPlayers);
        AllRooms[name] = std::weak_ptr(res);
        return res;
    }
    
    static std::vector<const TGameRoom*> GetAllRooms();

    bool AddPlayer(unsigned playerId);
    bool AddBot(std::unique_ptr<IBotStrategy>&& strategy);

    bool IsFull() const;

    void NotifyPlayerLeft(unsigned playerId);

    void Start();

    std::string HandleCommand(unsigned playerId, TSomeCommand cmd);
    std::string ExecuteRoomCommand(unsigned playerId, TRoomCommand cmd);
};
