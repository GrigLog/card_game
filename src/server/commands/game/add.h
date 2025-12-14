#pragma once

#include "../command.h"
#include "game_command.h"
#include "../../game_room.h"

// Команда добавления бота
struct AddCommand : GameCommand {
    std::unique_ptr<IBotStrategy> strategy;

    AddCommand(std::unique_ptr<IBotStrategy>&& strategy) : strategy(std::move(strategy)) {}
    
    virtual std::string gameExecute(GameRoom& room, unsigned playerId) override {
        if (room.bStarted) {
            return "error: The game has already started";
        }
        if (room.isFull()) {
            return "error: No free player slots";
        }
        room.addBot(std::move(strategy));
        return "ok: bot added";
    }
};

