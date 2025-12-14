#include "create.h"
#include "../game_room.h"

CreateCommand::CreateCommand(const std::string& name, size_t maxPlayers)
    : name(name), maxPlayers(maxPlayers) {
}

std::string CreateCommand::execute(
    unsigned playerId,
    std::unordered_map<unsigned, std::shared_ptr<GameRoom>>& playerToRoom
) {
    if (maxPlayers < 2 || maxPlayers > 6) {
        return "error: Invalid number of players (2-6)";
    }
    if (playerToRoom.contains(playerId)) {
        return "error: You are already in a room";
    }
    if (GameRoom::allRooms.contains(name)) {
        return "error: Room with this name already exists";
    }
    
    std::shared_ptr<GameRoom> room = GameRoom::make(name, playerId, maxPlayers);
    if (room.get()) {
        playerToRoom[playerId] = room;
        return std::format("ok: Room '{}' created. You are the owner. "\
            "Start the game when there is enough players. Commands available: {}", 
            name, Command::OWNER_COMMANDS_STR);
    } else {
        return "error: Failed to create room";
    }
}

