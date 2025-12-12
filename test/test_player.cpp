#include <gtest/gtest.h>
#include "card_game/HumanPlayer.h"
#include "card_game/BotPlayer.h"
#include "card_game/BotStrategy.h"
#include "card_game/Action.h"
#include "card_game/GameEvent.h"

using namespace card_game;

TEST(PlayerTest, HumanPlayerCreation) {
    HumanPlayer player(1, "TestPlayer");
    
    EXPECT_EQ(player.getId(), 1);
    EXPECT_EQ(player.getName(), "TestPlayer");
    EXPECT_TRUE(player.getHand().empty());
    EXPECT_FALSE(player.canPlay());
}

TEST(PlayerTest, HumanPlayerReceiveCards) {
    HumanPlayer player(1, "TestPlayer");
    
    std::vector<Card> cards = {
        Card(Suit::Hearts, Rank::Ace),
        Card(Suit::Spades, Rank::King)
    };
    
    player.receiveCards(cards);
    
    EXPECT_EQ(player.getHand().size(), 2);
    EXPECT_TRUE(player.canPlay());
}

TEST(PlayerTest, HumanPlayerSetAction) {
    HumanPlayer player(1, "TestPlayer");
    
    Card card(Suit::Hearts, Rank::Ace);
    player.setNextAction(std::make_unique<PlayCardAction>(card));
    
    GameEvent event(EventType::PlayerTurn, PlayerTurnEvent{1});
    auto action = player.makeMove(event);
    
    ASSERT_NE(action, nullptr);
    EXPECT_EQ(action->getType(), ActionType::PlayCard);
}

TEST(PlayerTest, BotPlayerCreation) {
    auto strategy = std::make_unique<RandomBotStrategy>();
    BotPlayer player(2, "Bot", std::move(strategy));
    
    EXPECT_EQ(player.getId(), 2);
    EXPECT_EQ(player.getName(), "Bot");
}

TEST(PlayerTest, BotPlayerMakeMove) {
    auto strategy = std::make_unique<RandomBotStrategy>();
    BotPlayer player(2, "Bot", std::move(strategy));
    
    std::vector<Card> cards = {
        Card(Suit::Hearts, Rank::Ace)
    };
    player.receiveCards(cards);
    
    GameEvent event(EventType::PlayerTurn, PlayerTurnEvent{2});
    auto action = player.makeMove(event);
    
    ASSERT_NE(action, nullptr);
    // Бот должен вернуть какое-то действие
    EXPECT_TRUE(action->getType() == ActionType::PlayCard || 
                action->getType() == ActionType::Pass);
}

TEST(PlayerTest, BotPlayerStrategyChange) {
    auto strategy1 = std::make_unique<RandomBotStrategy>();
    BotPlayer player(2, "Bot", std::move(strategy1));
    
    auto strategy2 = std::make_unique<LowCardBotStrategy>();
    player.setStrategy(std::move(strategy2));
    
    // Стратегия должна быть изменена
    std::vector<Card> cards = {
        Card(Suit::Hearts, Rank::Ace),
        Card(Suit::Spades, Rank::Six)
    };
    player.receiveCards(cards);
    
    GameEvent event(EventType::PlayerTurn, PlayerTurnEvent{2});
    auto action = player.makeMove(event);
    
    ASSERT_NE(action, nullptr);
    // LowCard стратегия должна играть самую маленькую карту
    if (action->getType() == ActionType::PlayCard) {
        auto* playAction = dynamic_cast<PlayCardAction*>(action.get());
        ASSERT_NE(playAction, nullptr);
        EXPECT_EQ(playAction->getCard().getRank(), Rank::Six);
    }
}

