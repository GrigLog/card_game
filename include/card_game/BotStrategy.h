#pragma once

#include "IPlayer.h"
#include "Action.h"
#include "Card.h"
#include <memory>
#include <vector>

namespace card_game {

// Паттерн Strategy: стратегия бота
class BotStrategy {
public:
    virtual ~BotStrategy() = default;
    virtual std::unique_ptr<Action> chooseAction(
        const std::vector<Card>& hand,
        const GameEvent& currentState
    ) = 0;
    virtual std::string getName() const = 0;
};

// Простая стратегия: играет случайную карту
class RandomBotStrategy : public BotStrategy {
public:
    std::unique_ptr<Action> chooseAction(
        const std::vector<Card>& hand,
        const GameEvent& currentState
    ) override;
    
    std::string getName() const override { return "Random"; }
};

// Стратегия: играет самую маленькую карту
class LowCardBotStrategy : public BotStrategy {
public:
    std::unique_ptr<Action> chooseAction(
        const std::vector<Card>& hand,
        const GameEvent& currentState
    ) override;
    
    std::string getName() const override { return "LowCard"; }
};

} // namespace card_game

