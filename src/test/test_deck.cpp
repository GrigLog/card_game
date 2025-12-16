#include <gtest/gtest.h>
#include "server/durak/deck.h"

TEST(DeckTest, DeckCreation) {
    TDeck deck;
    EXPECT_EQ(deck.Size(), 36); // Стандартная колода для Дурака
}

TEST(DeckTest, DealCards) {
    TDeck deck;
    const int count = 6;

    auto cards = deck.Deal(count);
    EXPECT_EQ(cards.size(), count);
    EXPECT_EQ(deck.Size(), 36 - count);
}

TEST(DeckTest, DealAllCards) {
    TDeck deck;
    auto allCards = deck.Deal(deck.Size());

    EXPECT_EQ(allCards.size(), 36);
}

TEST(DeckTest, DealMoreThanAvailable) {
    TDeck deck;
    auto cards = deck.Deal(deck.Size() * 2);

    EXPECT_EQ(cards.size(), 36);
    EXPECT_EQ(deck.Size(), 0);
}
