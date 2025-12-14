#pragma once

#include "card.h"
#include <vector>
#include <random>

class Deck {
    std::vector<Card> cards;
    std::mt19937 rng;

public:
    Deck();
    
    Card draw();
    
    bool empty() const { return cards.empty(); }
    
    // Количество оставшихся карт
    size_t size() const { return cards.size(); }
    
    // Раздать карты (возвращает вектор карт)
    std::vector<Card> deal(size_t count);
    
    // Показать нижнюю карту (будет козырной)
    Card getBottom() const;
};

