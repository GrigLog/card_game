#pragma once

#include <string>
#include <ostream>

namespace card_game {

enum class Suit {
    Hearts,   // Черви
    Diamonds, // Бубны
    Clubs,    // Трефы
    Spades    // Пики
};

enum class Rank {
    Six = 6,
    Seven,
    Eight,
    Nine,
    Ten,
    Jack,   // Валет
    Queen,  // Дама
    King,   // Король
    Ace     // Туз
};

class Card {
public:
    Card(Suit suit, Rank rank);
    
    Suit getSuit() const { return suit_; }
    Rank getRank() const { return rank_; }
    
    bool operator==(const Card& other) const;
    bool operator<(const Card& other) const;
    
    std::string toString() const;
    
private:
    Suit suit_;
    Rank rank_;
};

std::ostream& operator<<(std::ostream& os, const Card& card);
std::string suitToString(Suit suit);
std::string rankToString(Rank rank);

} // namespace card_game

