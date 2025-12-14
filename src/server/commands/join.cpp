#include "join.h"
#include "../game_room.h"

JoinCommand::JoinCommand(const std::string& name)
    : name(name) {
}

std::string JoinCommand::execute(
    unsigned playerId,
    std::unordered_map<unsigned, std::string>& playerIdToRoomId,
    std::unordered_map<std::string, std::unique_ptr<GameRoom>>& rooms
) {
    if (auto it = playerIdToRoomId.find(playerId); it != playerIdToRoomId.end()) {
        return "error: You are already in a room: " + it->second;
    }
    auto it = rooms.find(name);
    if (it == rooms.end()) {
        return "error: Room not found";
    }
    
    if (it->second->isFull()) {
        return "error: Room is full";
    }
    
    // Добавляем игрока в комнату
    if (it->second->addPlayer(playerId)) {
        return "ok: Joined room: " + name;
    } else {
        return "error: Failed to join room";
    }
}

