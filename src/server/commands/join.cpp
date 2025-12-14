#include "join.h"
#include "../game_room.h"

JoinCommand::JoinCommand(const std::string& name)
    : name(name) {
}

std::string JoinCommand::execute(
    unsigned playerId,
    std::unordered_map<unsigned, std::shared_ptr<GameRoom>>& playerToRoom
) {
    if (auto it = playerToRoom.find(playerId); it != playerToRoom.end()) {
        return "error: You are already in a room: " + it->second->name;
    }
    auto it = GameRoom::allRooms.find(name);
    if (it == GameRoom::allRooms.end() || it->second.expired()) {
        return "error: Room not found";
    }
    auto room = it->second.lock();
    if (room->isFull()) {
        return "error: Room is full";
    }
    
    // Добавляем игрока в комнату
    if (room->addPlayer(playerId)) {
        return "ok: Joined room: " + name;
    } else {
        return "error: Failed to join room";
    }
}

