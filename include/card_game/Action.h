#pragma once

#include "Card.h"
#include <memory>
#include <vector>
#include <optional>

namespace card_game {

// Паттерн Command: действия игроков
enum class ActionType {
    PlayCard,
    Pass,
    TakeCards
};

class Action {
public:
    virtual ~Action() = default;
    virtual ActionType getType() const = 0;
    virtual std::string toString() const = 0;
};

class PlayCardAction : public Action {
public:
    PlayCardAction(Card card);
    
    ActionType getType() const override { return ActionType::PlayCard; }
    Card getCard() const { return card_; }
    std::string toString() const override;
    
private:
    Card card_;
};

class PassAction : public Action {
public:
    PassAction();
    
    ActionType getType() const override { return ActionType::Pass; }
    std::string toString() const override;
};

class TakeCardsAction : public Action {
public:
    TakeCardsAction();
    
    ActionType getType() const override { return ActionType::TakeCards; }
    std::string toString() const override;
};

} // namespace card_game

