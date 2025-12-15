#include "strategy.h"

#include <string>
#include <algorithm>

#include "../durak/durak.h"


std::unique_ptr<IBotStrategy> IBotStrategy::parse(std::istream& iss) {
    std::string name;
    if (iss >> name) {
        if (name == "random") {
            return std::make_unique<RandomStrategy>();
        } else if (name == "sorted") {
            float coeff;
            if (iss >> coeff) {
                if (coeff < 0) {
                    coeff = 0;
                }
                if (coeff > 1) {
                    coeff = 1;
                }
                return std::make_unique<SortedStrategy>(coeff);
            }
        }
    }
    return nullptr;
}

GameCommand IBotStrategy::generateCommand(const DurakGame& game) {
    const auto& hand = game.hands[game.getActiveActor()];
    if (game.state == DurakState::AttackerThinks) {
        if (game.table.empty()) {
            return SelectCommand(selectFromAvailable(hand, game.trump) + 1);
        } else {
            std::vector<Card> filteredHand;
            for (const Card& c : hand) {
                if (c.canAttack(game.table)) {
                    filteredHand.push_back(c);
                }
            }
            int idx = selectFromAvailable(filteredHand, game.trump);
            if (idx == -1) {
                return EndCommand();
            }
            int realIdx = std::find(hand.begin(), hand.end(), filteredHand[idx]) - hand.begin();
            return SelectCommand(realIdx + 1);
        }
    } else {
        std::vector<Card> filteredHand;
        for (const Card& c : hand) {
            if (c.beats(game.attackingCard.value(), game.trump)) {
                filteredHand.push_back(c);
            }
        }
        int idx = selectFromAvailable(filteredHand, game.trump);
        if (idx == -1) {
            return TakeCommand();
        }
        int realIdx = std::find(hand.begin(), hand.end(), filteredHand[idx]) - hand.begin();
        return SelectCommand(realIdx + 1);
    }
}

int RandomStrategy::selectFromAvailable(const std::vector<Card>& hand, Suit trump) {
    if (hand.empty()) {
        return -1;
    }
    return 0; // Карты ж не отсортированы. Значит, брать первую - все равно, что случайную :)
}

int SortedStrategy::selectFromAvailable(const std::vector<Card>& hand, Suit trump) {
    if (hand.empty()) {
        return -1;
    }

    // Вектор пар: (индекс в исходной руке, приоритет карты)
    std::vector<std::pair<int, int>> cardPriorities;

    for (size_t i = 0; i < hand.size(); ++i) {
        const Card& card = hand[i];
        int priority = calculatePriority(card, trump);
        cardPriorities.emplace_back(i, priority);
    }

    // Сортируем по приоритету (чем меньше значение, тем слабее карта)
    std::sort(cardPriorities.begin(), cardPriorities.end(),
              [](const std::pair<int, int>& a, const std::pair<int, int>& b) {
                  return a.second < b.second;
              });

    // Возвращаем индекс подходящей по силе карты в исходном векторе
    int idx = cardPriorities.size() * coeff;
    return cardPriorities[idx].first;
}

int SortedStrategy::calculatePriority(const Card& card, Suit trump) {
    bool isTrump = (card.suit == trump);
    int basePriority = static_cast<int>(card.rank);

    // Базовый приоритет: сначала не козыри, потом козыри
    // Внутри каждой группы сортируем по возрастанию ранга
    if (!isTrump) {
        // Не козыри: младшие карты имеют меньший приоритет
        return basePriority;
    } else {
        // Козыри: размещаем после всех не козырей
        // Добавляем смещение, чтобы козыри были "сильнее"
        return basePriority + 100;
    }
}

GameCommand FallbackStrategy::generateCommand(const DurakGame& game) {
    if (game.state == DurakState::AttackerThinks) {
        if (game.table.empty()) {
            return SelectCommand(1);
        } else {
            return EndCommand();
        }
    } else {
        return TakeCommand();
    }
}
