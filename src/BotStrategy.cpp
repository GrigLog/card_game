#include "card_game/BotStrategy.h"
#include "card_game/Action.h"
#include <random>
#include <algorithm>
#include <chrono>

namespace card_game {

std::unique_ptr<Action> RandomBotStrategy::chooseAction(
    const std::vector<Card>& hand,
    const GameEvent& currentState) {
    
    if (hand.empty()) {
        return std::make_unique<PassAction>();
    }
    
    static std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());
    
    // Случайно выбираем между пасом и игрой карты
    if (rng() % 3 == 0) {
        return std::make_unique<PassAction>();
    }
    
    std::uniform_int_distribution<size_t> dist(0, hand.size() - 1);
    Card chosenCard = hand[dist(rng)];
    return std::make_unique<PlayCardAction>(chosenCard);
}

std::unique_ptr<Action> LowCardBotStrategy::chooseAction(
    const std::vector<Card>& hand,
    const GameEvent& currentState) {
    
    if (hand.empty()) {
        return std::make_unique<PassAction>();
    }
    
    // Находим самую маленькую карту
    Card minCard = *std::min_element(hand.begin(), hand.end());
    return std::make_unique<PlayCardAction>(minCard);
}

} // namespace card_game

