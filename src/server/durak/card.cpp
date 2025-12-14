#include "card.h"
#include <sstream>
#include <stdexcept>
#include <algorithm>

bool Card::beats(const Card& other, Suit trump) const {
    // Козырная карта бьет некозырную
    if (suit == trump && other.suit != trump) {
        return true;
    }
    if (suit != trump && other.suit == trump) {
        return false;
    }
    
    // Если обе козырные или обе некозырные - сравниваем по рангу
    // Но можно бить только картой той же масти или козырной
    if (suit == other.suit) {
        return rank > other.rank;
    }
    
    // Разные масти, обе некозырные - нельзя бить
    return false;
}

bool Card::canAttack(const std::vector<Card>& table) const {
    if (table.empty()) {
        return true; // Первая карта в атаке
    }
    
    // Можно атаковать картой того же ранга, что уже на столе
    for (const auto& card : table) {
        if (rank == card.rank) {
            return true;
        }
    }
    
    return false;
}

std::string Card::toString() const {
    std::string result;
    
    // Масть
    switch (suit) {
        case Suit::Spades: result += 'S'; break;
        case Suit::Clubs: result += 'C'; break;
        case Suit::Diamonds: result += 'D'; break;
        case Suit::Hearts: result += 'H'; break;
    }
    
    // Ранг
    if (rank >= Rank::Six && rank < Rank::Ten) {
        result += static_cast<char>('0' + static_cast<int>(rank));
    } else {
        switch (rank) {
            case Rank::Ten: result += "10"; break;
            case Rank::Jack: result += 'J'; break;
            case Rank::Queen: result += 'Q'; break;
            case Rank::King: result += 'K'; break;
            case Rank::Ace: result += 'A'; break;
            default: result += '?'; break;
        }
    }
    
    return result;
}

Card Card::fromString(const std::string& str) {
    if (str.length() < 2) {
        throw std::invalid_argument("Invalid card string: " + str);
    }
    
    Suit suit;
    switch (str[0]) {
        case 'S': case 's': suit = Suit::Spades; break;
        case 'C': case 'c': suit = Suit::Clubs; break;
        case 'D': case 'd': suit = Suit::Diamonds; break;
        case 'H': case 'h': suit = Suit::Hearts; break;
        default: throw std::invalid_argument("Invalid suit: " + str);
    }
    
    Rank rank;
    if (str[1] >= '6' && str[1] <= '9') {
        rank = static_cast<Rank>(str[1] - '0');
    } else if (str[1] == '1' && str.length() >= 3 && str[2] == '0') {
        rank = Rank::Ten;
    } else {
        switch (str[1]) {
            case 'J': case 'j': rank = Rank::Jack; break;
            case 'Q': case 'q': rank = Rank::Queen; break;
            case 'K': case 'k': rank = Rank::King; break;
            case 'A': case 'a': rank = Rank::Ace; break;
            default: throw std::invalid_argument("Invalid rank: " + str);
        }
    }
    
    return Card(suit, rank);
}

