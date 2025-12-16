#include "strategy.h"

#include <string>
#include <algorithm>

#include "../durak/durak.h"

std::unique_ptr<IBotStrategy> IBotStrategy::Parse(std::istream& iss) {
    std::string name;
    if (iss >> name) {
        if (name == "random") {
            return std::make_unique<TRandomStrategy>();
        } else if (name == "sorted") {
            float coeff;
            if (iss >> coeff) {
                if (coeff < 0) {
                    coeff = 0;
                }
                if (coeff > 1) {
                    coeff = 1;
                }
                return std::make_unique<TSortedStrategy>(coeff);
            }
        }
    }
    return nullptr;
}

TGameCommand IBotStrategy::GenerateCommand(const TDurakGame& game) {
    const auto& hand = game.Hands[game.GetActiveActor()];
    if (game.State == EDurakState::AttackerThinks) {
        if (game.Table.empty()) {
            return TSelectCommand(SelectFromAvailable(hand, game.Trump) + 1);
        } else {
            std::vector<TCard> filteredHand;
            for (const TCard& c : hand) {
                if (c.CanAttack(game.Table)) {
                    filteredHand.push_back(c);
                }
            }
            int idx = SelectFromAvailable(filteredHand, game.Trump);
            if (idx == -1) {
                return TEndCommand();
            }
            int realIdx = std::find(hand.begin(), hand.end(), filteredHand[idx]) - hand.begin();
            return TSelectCommand(realIdx + 1);
        }
    } else {
        std::vector<TCard> filteredHand;
        for (const TCard& c : hand) {
            if (c.Beats(game.AttackingCard.value(), game.Trump)) {
                filteredHand.push_back(c);
            }
        }
        int idx = SelectFromAvailable(filteredHand, game.Trump);
        if (idx == -1) {
            return TTakeCommand();
        }
        int realIdx = std::find(hand.begin(), hand.end(), filteredHand[idx]) - hand.begin();
        return TSelectCommand(realIdx + 1);
    }
}

int TRandomStrategy::SelectFromAvailable(const std::vector<TCard>& hand, ESuit trump) {
    if (hand.empty()) {
        return -1;
    }
    return 0; // Карты ж не отсортированы. Значит, брать первую - все равно, что случайную :)
}

int TSortedStrategy::SelectFromAvailable(const std::vector<TCard>& hand, ESuit trump) {
    if (hand.empty()) {
        return -1;
    }

    // Вектор пар: (индекс в исходной руке, приоритет карты)
    std::vector<std::pair<int, int>> cardPriorities;

    for (size_t i = 0; i < hand.size(); ++i) {
        const TCard& card = hand[i];
        int priority = CalculatePriority(card, trump);
        cardPriorities.emplace_back(i, priority);
    }

    // Сортируем по приоритету (чем меньше значение, тем слабее карта)
    std::sort(cardPriorities.begin(), cardPriorities.end(),
              [](const std::pair<int, int>& a, const std::pair<int, int>& b) {
                  return a.second < b.second;
              });

    // Возвращаем индекс подходящей по силе карты в исходном векторе
    int idx = cardPriorities.size() * Coeff;
    return cardPriorities[idx].first;
}

int TSortedStrategy::CalculatePriority(const TCard& card, ESuit trump) {
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

TGameCommand FallbackStrategy::GenerateCommand(const TDurakGame& game) {
    if (game.State == EDurakState::AttackerThinks) {
        if (game.Table.empty()) {
            return TSelectCommand(1);
        } else {
            return TEndCommand();
        }
    } else {
        return TTakeCommand();
    }
}
