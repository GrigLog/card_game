#pragma once

#include "command.h"

struct ListCommand : Command {
    std::string execute(
        unsigned playerId,
        std::unordered_map<unsigned, std::shared_ptr<GameRoom>>& playerToRoom
    ) override;
};

