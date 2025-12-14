#pragma once
#include "../game_room.h"

struct StartCommand : Command {
    std::string execute(
        unsigned playerId,
        std::unordered_map<unsigned, std::shared_ptr<GameRoom>>& playerToRoom
    ) override {
        auto it = playerToRoom.find(playerId);
        if (it == playerToRoom.end())
            return "error: You are not in a room.";
        auto room = it->second.get();
        if (room->ownerId != playerId)
            return "error: You must be the owner to start the game";
        
        room->bStarted = true;
        return "ok: Game started. It's your turn.";
    }
};