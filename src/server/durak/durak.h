#pragma once
#include <vector>
#include <memory>
#include "../actor/actor.h"
#include "../command.h"
#include "card.h"

enum class DurakState : uint8_t {
    AttackerThinks,
    DefenderThinks
};

struct DurakGame {
    std::vector<std::unique_ptr<IActor>> actors;
    Suit Trump;

    DurakState state = DurakState::AttackerThinks;
    unsigned attackingActor = 0;

    void processCommand(unsigned playerNum, std::unique_ptr<Command> command);
};