#include "deck.h"

#include <algorithm>
#include <random>
#include <chrono>

Deck::Deck()
    : rng(std::chrono::steady_clock::now().time_since_epoch().count()) {
    cards.reserve(36); // Стандартная колода для дурака

    // Создаем все карты от 6 до туза для каждой масти
    for (int s = 0; s < 4; ++s) {
        Suit suit = static_cast<Suit>(s);
        for (int r = 6; r <= 14; ++r) {
            cards.emplace_back(suit, static_cast<Rank>(r));
        }
    }

    std::shuffle(cards.begin(), cards.end(), rng);
}

Card Deck::draw() {
    if (cards.empty()) {
        throw std::runtime_error("Cannot draw from empty deck");
    }
    Card card = cards.back();
    cards.pop_back();
    return card;
}

std::vector<Card> Deck::deal(size_t count) {
    std::vector<Card> hand;
    hand.reserve(count);
    for (size_t i = 0; i < count && !cards.empty(); ++i) {
        hand.push_back(draw());
    }
    return hand;
}

Card Deck::getBottom() const {
    if (cards.empty()) {
        throw std::runtime_error("Cannot get trump from empty deck");
    }
    return cards[0];
}
