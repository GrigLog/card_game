#include "card_game/GameSession.h"
#include <algorithm>
#include <random>

namespace card_game {

GameSession::GameSession(std::vector<std::shared_ptr<IPlayer>> players)
    : players_(std::move(players)), currentPlayerId_(0), gameOver_(false) {
    
    if (players_.empty()) {
        throw std::invalid_argument("GameSession requires at least one player");
    }
}

void GameSession::start() {
    std::lock_guard<std::mutex> lock(stateMutex_);
    
    deck_.shuffle();
    dealInitialCards();
    
    // Определяем первого игрока (случайно)
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, static_cast<int>(players_.size()) - 1);
    currentPlayerId_ = dis(gen);
    
    gameOver_ = false;
    tableCards_.clear();
    
    notifyAll(GameEvent(EventType::GameStarted, PlayerTurnEvent{currentPlayerId_}));
    notifyAll(GameEvent(EventType::PlayerTurn, PlayerTurnEvent{currentPlayerId_}));
}

void GameSession::dealInitialCards() {
    const int cardsPerPlayer = 6;
    
    for (auto& player : players_) {
        auto cards = deck_.deal(cardsPerPlayer);
        player->receiveCards(std::move(cards));
        
        notifyAll(GameEvent(EventType::CardsDealt, 
            CardsDealtEvent{player->getId(), cards}));
    }
}

void GameSession::processAction(int playerId, std::unique_ptr<Action> action) {
    std::lock_guard<std::mutex> lock(stateMutex_);
    
    if (gameOver_) {
        return;
    }
    
    if (playerId != currentPlayerId_) {
        return; // Не очередь этого игрока
    }
    
    if (!action) {
        return;
    }
    
    switch (action->getType()) {
        case ActionType::PlayCard: {
            auto* playAction = dynamic_cast<PlayCardAction*>(action.get());
            if (playAction) {
                Card card = playAction->getCard();
                
                // Проверяем, есть ли карта у игрока
                auto& hand = players_[playerId]->getHand();
                auto it = std::find(hand.begin(), hand.end(), card);
                if (it != hand.end()) {
                    tableCards_.push_back(card);
                    hand.erase(it);
                    
                    notifyAll(GameEvent(EventType::CardPlayed, 
                        CardPlayedEvent{playerId, card}));
                    
                    // Добираем карты из колоды
                    if (!deck_.isEmpty()) {
                        auto newCards = deck_.deal(1);
                        players_[playerId]->receiveCards(std::move(newCards));
                    }
                    
                    nextTurn();
                }
            }
            break;
        }
        case ActionType::Pass: {
            notifyAll(GameEvent(EventType::TurnPassed, PlayerTurnEvent{playerId}));
            nextTurn();
            break;
        }
        case ActionType::TakeCards: {
            // Игрок берет карты со стола
            players_[playerId]->receiveCards(tableCards_);
            tableCards_.clear();
            nextTurn();
            break;
        }
    }
    
    checkGameOver();
}

void GameSession::nextTurn() {
    currentPlayerId_ = (currentPlayerId_ + 1) % static_cast<int>(players_.size());
    notifyAll(GameEvent(EventType::PlayerTurn, PlayerTurnEvent{currentPlayerId_}));
}

void GameSession::checkGameOver() {
    // Игра заканчивается, когда у кого-то закончились карты
    for (size_t i = 0; i < players_.size(); ++i) {
        if (players_[i]->getHand().empty() && deck_.isEmpty()) {
            gameOver_ = true;
            
            std::vector<int> losers;
            for (size_t j = 0; j < players_.size(); ++j) {
                if (j != i) {
                    losers.push_back(static_cast<int>(j));
                }
            }
            
            notifyAll(GameEvent(EventType::GameEnded, 
                GameEndedEvent{static_cast<int>(i), losers}));
            break;
        }
    }
}

void GameSession::notifyAll(const GameEvent& event) {
    for (auto& player : players_) {
        player->onEvent(event);
    }
}

std::vector<Card> GameSession::getTableCards() const {
    std::lock_guard<std::mutex> lock(stateMutex_);
    return tableCards_;
}

std::vector<Card> GameSession::getPlayerHand(int playerId) const {
    std::lock_guard<std::mutex> lock(stateMutex_);
    if (playerId >= 0 && playerId < static_cast<int>(players_.size())) {
        return players_[playerId]->getHand();
    }
    return {};
}

bool GameSession::canPlayCard(const Card& card, const std::vector<Card>& tableCards) const {
    if (tableCards.empty()) {
        return true; // Можно начать атаку любой картой
    }
    
    // Упрощенная логика: можно подкидывать карты того же ранга
    Rank firstRank = tableCards[0].getRank();
    return card.getRank() == firstRank;
}

bool GameSession::canDefend(const Card& attackCard, const Card& defenseCard) const {
    Suit trump = getTrumpSuit();
    
    // Если защита козырем, а атака не козырем - можно
    if (defenseCard.getSuit() == trump && attackCard.getSuit() != trump) {
        return true;
    }
    
    // Если обе карты одной масти, защита должна быть выше
    if (defenseCard.getSuit() == attackCard.getSuit()) {
        return defenseCard.getRank() > attackCard.getRank();
    }
    
    return false;
}

Suit GameSession::getTrumpSuit() const {
    // В упрощенной версии козырь - последняя карта в колоде
    // Для простоты используем Hearts как козырь
    return Suit::Hearts;
}

} // namespace card_game

