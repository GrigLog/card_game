#include "card_game/HumanPlayer.h"
#include "card_game/Action.h"
#include <algorithm>

namespace card_game {

HumanPlayer::HumanPlayer(int id, std::string name)
    : id_(id), name_(std::move(name)) {}

void HumanPlayer::onEvent(const GameEvent& event) {
    // В реальной реализации здесь будет обработка сетевых событий
}

void HumanPlayer::receiveCards(std::vector<Card> cards) {
    std::lock_guard<std::mutex> lock(mutex_);
    hand_.insert(hand_.end(), cards.begin(), cards.end());
}

std::unique_ptr<Action> HumanPlayer::makeMove(const GameEvent& currentState) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!queuedActions_.empty()) {
        auto action = std::move(queuedActions_.front());
        queuedActions_.pop();
        return action;
    }
    
    // По умолчанию пас
    return std::make_unique<PassAction>();
}

void HumanPlayer::setNextAction(std::unique_ptr<Action> action) {
    std::lock_guard<std::mutex> lock(mutex_);
    queuedActions_.push(std::move(action));
}

void HumanPlayer::clearActions() {
    std::lock_guard<std::mutex> lock(mutex_);
    while (!queuedActions_.empty()) {
        queuedActions_.pop();
    }
}

} // namespace card_game

