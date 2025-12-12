#include <gtest/gtest.h>
#include "card_game/Action.h"

using namespace card_game;

TEST(ActionTest, PlayCardAction) {
    Card card(Suit::Hearts, Rank::Ace);
    PlayCardAction action(card);
    
    EXPECT_EQ(action.getType(), ActionType::PlayCard);
    EXPECT_EQ(action.getCard(), card);
    EXPECT_FALSE(action.toString().empty());
}

TEST(ActionTest, PassAction) {
    PassAction action;
    
    EXPECT_EQ(action.getType(), ActionType::Pass);
    EXPECT_FALSE(action.toString().empty());
}

TEST(ActionTest, TakeCardsAction) {
    TakeCardsAction action;
    
    EXPECT_EQ(action.getType(), ActionType::TakeCards);
    EXPECT_FALSE(action.toString().empty());
}

