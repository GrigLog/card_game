#include <gtest/gtest.h>
#include "card_game/Deck.h"

using namespace card_game;

TEST(DeckTest, DeckCreation) {
    Deck deck;
    EXPECT_FALSE(deck.isEmpty());
    EXPECT_EQ(deck.size(), 36); // Стандартная колода для Дурака
}

TEST(DeckTest, DeckShuffle) {
    Deck deck1;
    Deck deck2;
    
    deck1.shuffle();
    deck2.shuffle();
    
    // После перемешивания порядок должен быть разным (с высокой вероятностью)
    // Проверяем, что колода не пустая
    EXPECT_FALSE(deck1.isEmpty());
    EXPECT_FALSE(deck2.isEmpty());
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
    auto allCards = deck.takeAll();
    
    EXPECT_EQ(allCards.size(), 36);
    EXPECT_TRUE(deck.isEmpty());
}

TEST(DeckTest, DealMoreThanAvailable) {
    Deck deck;
    auto cards = deck.deal(100); // Больше чем есть в колоде
    
    EXPECT_EQ(cards.size(), 36);
    EXPECT_TRUE(deck.isEmpty());
}

