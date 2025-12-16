#pragma once

#include <string>
#include <vector>
#include <cstdint>

enum class ESuit : uint8_t {
    Spades = 0,   // Пики
    Clubs = 1,    // Трефы
    Diamonds = 2, // Бубны
    Hearts = 3    // Червы
};

enum class ERank : uint8_t {
    Six = 6,
    Seven = 7,
    Eight = 8,
    Nine = 9,
    Ten = 10,
    Jack = 11,  // Валет
    Queen = 12, // Дама
    King = 13,  // Король
    Ace = 14    // Туз
};

class TCard {
public:
    ESuit suit;
    ERank rank;

public:
    TCard(ESuit suit, ERank rank)
        : suit(suit)
        , rank(rank) 
    {}

    bool operator==(const TCard& other) const {
        return suit == other.suit && rank == other.rank;
    }

    // Проверка, бьет ли эта карта другую (с учетом козырной масти)
    bool Beats(const TCard& other, ESuit trump) const;

    // Проверка, можно ли атаковать этой картой (совпадает ли ранг с уже выложенными)
    bool CanAttack(const std::vector<TCard>& table) const;

    // Сериализация в строку (формат: "S6" для пики 6, "HA" для червового туза)
    std::string ToString() const;

    // Десериализация из строки
    static TCard FromString(const std::string& str);
};
