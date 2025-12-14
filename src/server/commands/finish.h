#pragma once
#include "command.h"
#include "../game_room.h"
#include "../actor/player.h"

struct FinishCommand : Command {
    std::string execute(
        unsigned playerId,
        std::unordered_map<unsigned, std::shared_ptr<GameRoom>>& playerToRoom
    ) override {
        auto it = playerToRoom.find(playerId);
        if (it == playerToRoom.end())
            return "error: You are not in a room.";
        auto room = it->second;
        if (room->ownerId != playerId)
            return "error: You must be the owner to close the room";
        for (const auto& actor : room->actors) {
            if (auto player = dynamic_cast<Player*>(actor.get())) {
                playerToRoom.erase(player->id);
            }
        }
        return "Room closed. All players have been sent back to lobby (including you).";
    }
};