#pragma once

#include "IPlayer.h"
#include "BotStrategy.h"
#include <memory>
#include <vector>

namespace card_game {

// Бот-игрок с выбранной стратегией
class BotPlayer : public IPlayer {
public:
    BotPlayer(int id, std::string name, std::unique_ptr<BotStrategy> strategy);
    
    int getId() const override { return id_; }
    std::string getName() const override { return name_; }
    
    void onEvent(const GameEvent& event) override;
    void receiveCards(std::vector<Card> cards) override;
    const std::vector<Card>& getHand() const override { return hand_; }
    
    std::unique_ptr<Action> makeMove(const GameEvent& currentState) override;
    bool canPlay() const override { return !hand_.empty(); }
    
    void setStrategy(std::unique_ptr<BotStrategy> strategy);
    
private:
    int id_;
    std::string name_;
    std::vector<Card> hand_;
    std::unique_ptr<BotStrategy> strategy_;
    GameEvent lastEvent_;
};

} // namespace card_game

