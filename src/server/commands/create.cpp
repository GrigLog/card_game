#include "create.h"
#include "../game_room.h"

CreateCommand::CreateCommand(const std::string& name, size_t maxPlayers)
    : name(name), maxPlayers(maxPlayers) {
}

std::string CreateCommand::execute(
    unsigned playerId,
    std::unordered_map<unsigned, std::string>& playerIdToRoomId,
    std::unordered_map<std::string, std::unique_ptr<GameRoom>>& rooms
) {
    if (maxPlayers < 2 || maxPlayers > 6) {
        return "error: Invalid number of players (2-6)";
    }
    if (playerIdToRoomId.contains(playerId)) {
        return "error: You are already in a room";
    }
    if (rooms.find(name) != rooms.end()) {
        return "error: Room already exists";
    }
    
    std::unique_ptr<GameRoom> room = std::make_unique<GameRoom>(name, playerId, maxPlayers);
    if (room.get()) {
        rooms[name] = std::move(room);
        playerIdToRoomId[playerId] = name;
        return std::format("ok: Room '{}' created. You are the owner. "\
            "Start the game when there is enough players. Commands available: {}", 
            name, Command::OWNER_COMMANDS_STR);
    } else {
        return "error: Failed to create room";
    }
}

