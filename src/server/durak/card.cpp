#include "card.h"
#include <sstream>
#include <stdexcept>
#include <algorithm>

bool TCard::Beats(const TCard& other, ESuit trump) const {
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

bool TCard::CanAttack(const std::vector<TCard>& table) const {
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

std::string TCard::ToString() const {
    std::string result;

    // Масть
    switch (suit) {
        case ESuit::Spades:
            result += 'S';
            break;
        case ESuit::Clubs:
            result += 'C';
            break;
        case ESuit::Diamonds:
            result += 'D';
            break;
        case ESuit::Hearts:
            result += 'H';
            break;
    }

    // Ранг
    if (rank >= ERank::Six && rank < ERank::Ten) {
        result += static_cast<char>('0' + static_cast<int>(rank));
    } else {
        switch (rank) {
            case ERank::Ten:
                result += "10";
                break;
            case ERank::Jack:
                result += 'J';
                break;
            case ERank::Queen:
                result += 'Q';
                break;
            case ERank::King:
                result += 'K';
                break;
            case ERank::Ace:
                result += 'A';
                break;
            default:
                result += '?';
                break;
        }
    }

    return result;
}

TCard TCard::FromString(const std::string& str) {
    if (str.length() < 2) {
        throw std::invalid_argument("Invalid card string: " + str);
    }

    ESuit suit;
    switch (str[0]) {
        case 'S':
        case 's':
            suit = ESuit::Spades;
            break;
        case 'C':
        case 'c':
            suit = ESuit::Clubs;
            break;
        case 'D':
        case 'd':
            suit = ESuit::Diamonds;
            break;
        case 'H':
        case 'h':
            suit = ESuit::Hearts;
            break;
        default:
            throw std::invalid_argument("Invalid suit: " + str);
    }

    ERank rank;
    if (str[1] >= '6' && str[1] <= '9') {
        rank = static_cast<ERank>(str[1] - '0');
    } else if (str[1] == '1' && str.length() >= 3 && str[2] == '0') {
        rank = ERank::Ten;
    } else {
        switch (str[1]) {
            case 'J':
            case 'j':
                rank = ERank::Jack;
                break;
            case 'Q':
            case 'q':
                rank = ERank::Queen;
                break;
            case 'K':
            case 'k':
                rank = ERank::King;
                break;
            case 'A':
            case 'a':
                rank = ERank::Ace;
                break;
            default:
                throw std::invalid_argument("Invalid rank: " + str);
        }
    }

    return TCard(suit, rank);
}
