#pragma once

#include <string>
#include <vector>
#include <cstdint>

enum class Suit : uint8_t {
    Spades = 0,    // Пики
    Clubs = 1,     // Трефы
    Diamonds = 2,  // Бубны
    Hearts = 3     // Червы
};

enum class Rank : uint8_t {
    Six = 6,
    Seven = 7,
    Eight = 8,
    Nine = 9,
    Ten = 10,
    Jack = 11,     // Валет
    Queen = 12,    // Дама
    King = 13,     // Король
    Ace = 14       // Туз
};

struct Card {
    Suit suit;
    Rank rank;

    Card(Suit suit, Rank rank) : suit(suit), rank(rank) {}
    
    // Проверка, бьет ли эта карта другую (с учетом козырной масти)
    bool beats(const Card& other, Suit trump) const;
    
    // Проверка, можно ли атаковать этой картой (совпадает ли ранг с уже выложенными)
    bool canAttack(const std::vector<Card>& table) const;
    
    // Сериализация в строку (формат: "S6" для пики 6, "HA" для червового туза)
    std::string toString() const;
    
    // Десериализация из строки
    static Card fromString(const std::string& str);
    
    bool operator==(const Card& other) const {
        return suit == other.suit && rank == other.rank;
    }
    
    // bool operator<(const Card& other) const {
    //     if (rank != other.rank) 
    //         return rank < other.rank;
    //     return suit < other.suit;
    // }
};

