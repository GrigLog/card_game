#pragma once

#include "command.h"

// Команда создания комнаты
struct CreateCommand : Command {
    std::string name;
    size_t maxPlayers;

    CreateCommand(const std::string& name, size_t maxPlayers);
    
    std::string execute(
        unsigned playerId,
        std::unordered_map<unsigned, std::string>& playerIdToRoomId,
        std::unordered_map<std::string, std::unique_ptr<GameRoom>>& rooms
    ) override;
};

