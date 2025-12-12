#include <gtest/gtest.h>
#include "card_game/Card.h"

using namespace card_game;

TEST(CardTest, CardCreation) {
    Card card(Suit::Hearts, Rank::Ace);
    EXPECT_EQ(card.getSuit(), Suit::Hearts);
    EXPECT_EQ(card.getRank(), Rank::Ace);
}

TEST(CardTest, CardEquality) {
    Card card1(Suit::Hearts, Rank::Ace);
    Card card2(Suit::Hearts, Rank::Ace);
    Card card3(Suit::Spades, Rank::Ace);
    
    EXPECT_EQ(card1, card2);
    EXPECT_NE(card1, card3);
}

TEST(CardTest, CardComparison) {
    Card card1(Suit::Hearts, Rank::Six);
    Card card2(Suit::Hearts, Rank::Ace);
    
    EXPECT_LT(card1, card2);
}

TEST(CardTest, CardToString) {
    Card card(Suit::Hearts, Rank::Ace);
    std::string str = card.toString();
    EXPECT_FALSE(str.empty());
}

