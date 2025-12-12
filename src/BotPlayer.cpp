#include "card_game/BotPlayer.h"
#include "card_game/Action.h"
#include <algorithm>

namespace card_game {

BotPlayer::BotPlayer(int id, std::string name, std::unique_ptr<BotStrategy> strategy)
    : id_(id), name_(std::move(name)), strategy_(std::move(strategy)) {}

void BotPlayer::onEvent(const GameEvent& event) {
    lastEvent_ = event;
}

void BotPlayer::receiveCards(std::vector<Card> cards) {
    hand_.insert(hand_.end(), cards.begin(), cards.end());
}

std::unique_ptr<Action> BotPlayer::makeMove(const GameEvent& currentState) {
    if (hand_.empty()) {
        return std::make_unique<PassAction>();
    }
    
    return strategy_->chooseAction(hand_, currentState);
}

void BotPlayer::setStrategy(std::unique_ptr<BotStrategy> strategy) {
    strategy_ = std::move(strategy);
}

} // namespace card_game

