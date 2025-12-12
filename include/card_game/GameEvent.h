#pragma once

#include "Card.h"
#include <string>
#include <vector>
#include <memory>
#include <variant>

namespace card_game {

// Паттерн Observer: события игры
enum class EventType {
    CardPlayed,
    TurnPassed,
    CardsDealt,
    GameStarted,
    GameEnded,
    PlayerTurn,
    AttackStarted,
    DefenseStarted
};

struct CardPlayedEvent {
    int playerId;
    Card card;
};

struct CardsDealtEvent {
    int playerId;
    std::vector<Card> cards;
};

struct GameEndedEvent {
    int winnerId;
    std::vector<int> losers;
};

struct PlayerTurnEvent {
    int playerId;
};

// Вариант для всех типов событий
using EventData = std::variant<
    CardPlayedEvent,
    CardsDealtEvent,
    GameEndedEvent,
    PlayerTurnEvent
>;

class GameEvent {
public:
    GameEvent(EventType type, EventData data);
    
    EventType getType() const { return type_; }
    const EventData& getData() const { return data_; }
    
    std::string toString() const;
    
private:
    EventType type_;
    EventData data_;
};

} // namespace card_game

