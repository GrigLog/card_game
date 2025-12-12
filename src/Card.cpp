#include "card_game/Card.h"
#include <sstream>

namespace card_game {

Card::Card(Suit suit, Rank rank) : suit_(suit), rank_(rank) {}

bool Card::operator==(const Card& other) const {
    return suit_ == other.suit_ && rank_ == other.rank_;
}

bool Card::operator<(const Card& other) const {
    if (rank_ != other.rank_) {
        return rank_ < other.rank_;
    }
    return suit_ < other.suit_;
}

std::string Card::toString() const {
    return rankToString(rank_) + " " + suitToString(suit_);
}

std::ostream& operator<<(std::ostream& os, const Card& card) {
    os << card.toString();
    return os;
}

std::string suitToString(Suit suit) {
    switch (suit) {
        case Suit::Hearts:   return "♥";
        case Suit::Diamonds: return "♦";
        case Suit::Clubs:    return "♣";
        case Suit::Spades:   return "♠";
        default:             return "?";
    }
}

std::string rankToString(Rank rank) {
    switch (rank) {
        case Rank::Six:   return "6";
        case Rank::Seven: return "7";
        case Rank::Eight: return "8";
        case Rank::Nine:  return "9";
        case Rank::Ten:   return "10";
        case Rank::Jack:  return "J";
        case Rank::Queen: return "Q";
        case Rank::King:  return "K";
        case Rank::Ace:   return "A";
        default:          return "?";
    }
}

} // namespace card_game

