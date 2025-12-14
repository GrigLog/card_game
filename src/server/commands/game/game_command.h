#pragma once
#include "../command.h"

struct GameCommand : Command {
    std::string execute(
        unsigned playerId,
        std::unordered_map<unsigned, std::string>& playerIdToRoomId,
        std::unordered_map<std::string, std::unique_ptr<GameRoom>>& rooms
    ) override {
        auto it = playerIdToRoomId.find(playerId);
        if (it == playerIdToRoomId.end()) {
            return "error: Can't play without joining a room";
        }
        std::string name = it->second;
        auto it2 = rooms.find(name);
        if (it2 == rooms.end()) {
            return "error: room name '" + name + "' exists, but the room is absent. Weird.";
        }
        GameRoom& room = *it2->second.get();
        return gameExecute(room, playerId);
    }

    virtual std::string gameExecute(GameRoom& room, unsigned playerId) = 0;
};