#pragma once
#include "../command.h"

struct GameCommand : Command {
    std::string execute(
        unsigned playerId,
        std::unordered_map<unsigned, std::shared_ptr<GameRoom>>& playerToRoom
    ) override {
        auto it = playerToRoom.find(playerId);
        if (it == playerToRoom.end()) {
            return "error: Can't play without joining a room";
        }
        GameRoom* room = it->second.get();
        return gameExecute(*room, playerId);
    }

    virtual std::string gameExecute(GameRoom& room, unsigned playerId) = 0;
};