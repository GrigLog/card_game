#include "card_game/Deck.h"
#include <algorithm>
#include <random>
#include <chrono>

namespace card_game {

Deck::Deck() : rng_(std::chrono::steady_clock::now().time_since_epoch().count()) {
    initialize();
}

void Deck::initialize() {
    cards_.clear();
    cards_.reserve(36); // Стандартная колода для игры в Дурака
    
    for (int s = static_cast<int>(Suit::Hearts); s <= static_cast<int>(Suit::Spades); ++s) {
        for (int r = static_cast<int>(Rank::Six); r <= static_cast<int>(Rank::Ace); ++r) {
            cards_.emplace_back(static_cast<Suit>(s), static_cast<Rank>(r));
        }
    }
}

void Deck::shuffle() {
    std::shuffle(cards_.begin(), cards_.end(), rng_);
}

std::vector<Card> Deck::deal(int count) {
    if (count > static_cast<int>(cards_.size())) {
        count = static_cast<int>(cards_.size());
    }
    
    std::vector<Card> dealt;
    dealt.reserve(count);
    
    for (int i = 0; i < count; ++i) {
        dealt.push_back(cards_.back());
        cards_.pop_back();
    }
    
    return dealt;
}

std::vector<Card> Deck::takeAll() {
    std::vector<Card> result;
    result.swap(cards_);
    return result;
}

} // namespace card_game

