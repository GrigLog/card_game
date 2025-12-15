#include <gtest/gtest.h>
#include "server/durak/deck.h"

TEST(DeckTest, DeckCreation) {
    Deck deck;
    EXPECT_EQ(deck.size(), 36); // Стандартная колода для Дурака
}

TEST(DeckTest, DealCards) {
    Deck deck;
    const int count = 6;

    auto cards = deck.deal(count);
    EXPECT_EQ(cards.size(), count);
    EXPECT_EQ(deck.size(), 36 - count);
}

TEST(DeckTest, DealAllCards) {
    Deck deck;
    auto allCards = deck.deal(deck.size());

    EXPECT_EQ(allCards.size(), 36);
}

TEST(DeckTest, DealMoreThanAvailable) {
    Deck deck;
    auto cards = deck.deal(deck.size() * 2);

    EXPECT_EQ(cards.size(), 36);
    EXPECT_EQ(deck.size(), 0);
}
