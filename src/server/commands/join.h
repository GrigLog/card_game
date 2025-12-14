#pragma once

#include "command.h"

// Команда присоединения к комнате
struct JoinCommand : Command {
    std::string name;

    JoinCommand(const std::string& name);
    
    std::string execute(
        unsigned playerId,
        std::unordered_map<unsigned, std::shared_ptr<GameRoom>>& playerToRoom
    ) override;
};

