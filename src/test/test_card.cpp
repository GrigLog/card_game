#include <gtest/gtest.h>
#include "server/durak/card.h"

TEST(CardTest, CardCreation) {
    TCard card(ESuit::Hearts, ERank::Ace);
    EXPECT_EQ(card.suit, ESuit::Hearts);
    EXPECT_EQ(card.rank, ERank::Ace);
}

TEST(CardTest, CardEquality) {
    TCard card1(ESuit::Hearts, ERank::Ace);
    TCard card2(ESuit::Hearts, ERank::Ace);
    TCard card3(ESuit::Spades, ERank::Ace);

    EXPECT_EQ(card1, card2);
    EXPECT_NE(card1, card3);
}

TEST(CardTest, CardComparison) {
    TCard trumpSix(ESuit::Hearts, ERank::Six);
    TCard ace(ESuit::Spades, ERank::Ace);
    TCard queen(ESuit::Spades, ERank::Queen);

    EXPECT_TRUE(ace.Beats(queen, ESuit::Hearts));
    EXPECT_TRUE(trumpSix.Beats(ace, ESuit::Hearts));
}

TEST(CardTest, CardToString) {
    TCard card(ESuit::Hearts, ERank::Ace);
    std::string str = card.ToString();
    EXPECT_STREQ(card.ToString().c_str(), "HA");
}
