#pragma once

#include "IPlayer.h"
#include <memory>
#include <vector>
#include <queue>
#include <mutex>

namespace card_game {

// Человеческий игрок (для тестирования с mock-объектами)
class HumanPlayer : public IPlayer {
public:
    HumanPlayer(int id, std::string name);
    
    int getId() const override { return id_; }
    std::string getName() const override { return name_; }
    
    void onEvent(const GameEvent& event) override;
    void receiveCards(std::vector<Card> cards) override;
    const std::vector<Card>& getHand() const override { return hand_; }
    
    std::unique_ptr<Action> makeMove(const GameEvent& currentState) override;
    bool canPlay() const override { return !hand_.empty(); }
    
    // Методы для тестирования: установка действия заранее
    void setNextAction(std::unique_ptr<Action> action);
    void clearActions();
    
private:
    int id_;
    std::string name_;
    std::vector<Card> hand_;
    std::queue<std::unique_ptr<Action>> queuedActions_;
    mutable std::mutex mutex_;
};

} // namespace card_game

