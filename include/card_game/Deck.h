#pragma once

#include "Card.h"
#include <vector>
#include <memory>
#include <random>

namespace card_game {

class Deck {
public:
    Deck();
    
    void shuffle();
    std::vector<Card> deal(int count);
    bool isEmpty() const { return cards_.empty(); }
    size_t size() const { return cards_.size(); }
    
    // Семантика перемещения для больших структур
    std::vector<Card> takeAll();
    
private:
    std::vector<Card> cards_;
    std::mt19937 rng_;
    
    void initialize();
};

} // namespace card_game

