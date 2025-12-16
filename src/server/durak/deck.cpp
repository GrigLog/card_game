#include "deck.h"

#include <algorithm>
#include <random>
#include <chrono>

TDeck::TDeck()
    : Rng(std::chrono::steady_clock::now().time_since_epoch().count()) {
    Cards.reserve(36); // Стандартная колода для дурака

    // Создаем все карты от 6 до туза для каждой масти
    for (int s = 0; s < 4; ++s) {
        ESuit suit = static_cast<ESuit>(s);
        for (int r = 6; r <= 14; ++r) {
            Cards.emplace_back(suit, static_cast<ERank>(r));
        }
    }

    std::shuffle(Cards.begin(), Cards.end(), Rng);
}

TCard TDeck::draw() {
    if (Cards.empty()) {
        throw std::runtime_error("Cannot draw from empty deck");
    }
    TCard card = Cards.back();
    Cards.pop_back();
    return card;
}

std::vector<TCard> TDeck::Deal(size_t count) {
    std::vector<TCard> hand;
    hand.reserve(count);
    for (size_t i = 0; i < count && !Cards.empty(); ++i) {
        hand.push_back(draw());
    }
    return hand;
}

TCard TDeck::GetBottom() const {
    if (Cards.empty()) {
        throw std::runtime_error("Cannot get trump from empty deck");
    }
    return Cards[0];
}
