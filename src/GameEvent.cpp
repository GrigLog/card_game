#include "card_game/GameEvent.h"
#include <sstream>

namespace card_game {

GameEvent::GameEvent(EventType type, EventData data) 
    : type_(type), data_(std::move(data)) {}

std::string GameEvent::toString() const {
    std::ostringstream oss;
    
    switch (type_) {
        case EventType::CardPlayed:
            if (auto* event = std::get_if<CardPlayedEvent>(&data_)) {
                oss << "CardPlayed: Player " << event->playerId 
                    << " played " << event->card.toString();
            }
            break;
        case EventType::TurnPassed:
            oss << "TurnPassed";
            break;
        case EventType::CardsDealt:
            if (auto* event = std::get_if<CardsDealtEvent>(&data_)) {
                oss << "CardsDealt: Player " << event->playerId 
                    << " received " << event->cards.size() << " cards";
            }
            break;
        case EventType::GameStarted:
            oss << "GameStarted";
            break;
        case EventType::GameEnded:
            if (auto* event = std::get_if<GameEndedEvent>(&data_)) {
                oss << "GameEnded: Winner is Player " << event->winnerId;
            }
            break;
        case EventType::PlayerTurn:
            if (auto* event = std::get_if<PlayerTurnEvent>(&data_)) {
                oss << "PlayerTurn: Player " << event->playerId;
            }
            break;
        case EventType::AttackStarted:
            oss << "AttackStarted";
            break;
        case EventType::DefenseStarted:
            oss << "DefenseStarted";
            break;
    }
    
    return oss.str();
}

} // namespace card_game

