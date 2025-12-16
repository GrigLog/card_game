#pragma once

#include <vector>
#include <random>

#include "card.h"

class TDeck {
    std::vector<TCard> Cards;
    std::mt19937 Rng;

public:
    TDeck();

    TCard draw();

    bool IsEmpty() const {
        return Cards.empty();
    }

    // Количество оставшихся карт
    size_t Size() const {
        return Cards.size();
    }

    // Раздать карты (возвращает вектор карт)
    std::vector<TCard> Deal(size_t count);

    // Показать нижнюю карту (будет козырной)
    TCard GetBottom() const;
};
