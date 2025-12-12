#pragma once

#include "IPlayer.h"
#include "Deck.h"
#include "GameEvent.h"
#include "Action.h"
#include <vector>
#include <memory>
#include <mutex>
#include <atomic>

namespace card_game {

// Игровая сессия с логикой игры в Дурака
class GameSession {
public:
    explicit GameSession(std::vector<std::shared_ptr<IPlayer>> players);
    
    void start();
    void processAction(int playerId, std::unique_ptr<Action> action);
    
    bool isGameOver() const { return gameOver_; }
    int getCurrentPlayerId() const { return currentPlayerId_; }
    
    // Получение текущего состояния для отображения
    std::vector<Card> getTableCards() const;
    std::vector<Card> getPlayerHand(int playerId) const;
    
private:
    std::vector<std::shared_ptr<IPlayer>> players_;
    Deck deck_;
    std::vector<Card> tableCards_;
    std::atomic<int> currentPlayerId_;
    std::atomic<bool> gameOver_;
    mutable std::mutex stateMutex_;
    
    // Логика игры
    void dealInitialCards();
    void notifyAll(const GameEvent& event);
    void nextTurn();
    void checkGameOver();
    
    // Правила игры в Дурака
    bool canPlayCard(const Card& card, const std::vector<Card>& tableCards) const;
    bool canDefend(const Card& attackCard, const Card& defenseCard) const;
    Suit getTrumpSuit() const;
};

} // namespace card_game

