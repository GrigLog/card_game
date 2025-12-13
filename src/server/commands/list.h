#pragma once

#include "command.h"

struct ListCommand : Command {
    std::string execute(
        unsigned playerId,
        std::unordered_map<unsigned, std::string>& playerIdToRoomId,
        std::unordered_map<std::string, std::unique_ptr<GameRoom>>& rooms
    ) override;
};

