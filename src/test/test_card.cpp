#include <gtest/gtest.h>
#include "server/durak/card.h"

TEST(CardTest, CardCreation) {
    Card card(Suit::Hearts, Rank::Ace);
    EXPECT_EQ(card.suit, Suit::Hearts);
    EXPECT_EQ(card.rank, Rank::Ace);
}

TEST(CardTest, CardEquality) {
    Card card1(Suit::Hearts, Rank::Ace);
    Card card2(Suit::Hearts, Rank::Ace);
    Card card3(Suit::Spades, Rank::Ace);

    EXPECT_EQ(card1, card2);
    EXPECT_NE(card1, card3);
}

TEST(CardTest, CardComparison) {
    Card trumpSix(Suit::Hearts, Rank::Six);
    Card ace(Suit::Spades, Rank::Ace);
    Card queen(Suit::Spades, Rank::Queen);

    EXPECT_TRUE(ace.beats(queen, Suit::Hearts));
    EXPECT_TRUE(trumpSix.beats(ace, Suit::Hearts));
}

TEST(CardTest, CardToString) {
    Card card(Suit::Hearts, Rank::Ace);
    std::string str = card.toString();
    EXPECT_STREQ(card.toString().c_str(), "HA");
}
