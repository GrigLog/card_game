#pragma once

#include "GameEvent.h"
#include "Action.h"
#include "Card.h"
#include <memory>
#include <vector>

namespace card_game {

// Интерфейс игрока (Observer для получения событий)
class IPlayer {
public:
    virtual ~IPlayer() = default;
    
    virtual int getId() const = 0;
    virtual std::string getName() const = 0;
    
    // Observer: уведомление о событиях игры
    virtual void onEvent(const GameEvent& event) = 0;
    
    // Получение карт
    virtual void receiveCards(std::vector<Card> cards) = 0;
    virtual const std::vector<Card>& getHand() const = 0;
    
    // Запрос действия от игрока
    virtual std::unique_ptr<Action> makeMove(const GameEvent& currentState) = 0;
    
    // Проверка возможности хода
    virtual bool canPlay() const = 0;
};

} // namespace card_game

