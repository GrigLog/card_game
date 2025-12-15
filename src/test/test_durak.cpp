#include <gtest/gtest.h>

#include <memory>
#include <vector>

#include "server/actor/actor.h"
#include "server/durak/durak.h"

TEST(DurakTest, PlayerAttackPlayerTake) {
    std::vector<std::unique_ptr<IActor>> actors;
    actors.emplace_back(new Player{0, true});
    actors.emplace_back(new Player{1, false});
    // broadcasts won't fail because PlayerManager has a safety check in sendToPlayer
    DurakGame game(actors);

    game.executePlayerGameCommand(0, SelectCommand(1));
    EXPECT_EQ(game.state, DurakState::DefenderThinks);
    EXPECT_TRUE(game.attackingCard.has_value());
    game.executePlayerGameCommand(1, TakeCommand());
    EXPECT_EQ(game.hands[0].size(), 6);
    EXPECT_EQ(game.hands[1].size(), 7);
    EXPECT_EQ(game.attackingActor, 0);
    EXPECT_EQ(game.state, DurakState::AttackerThinks);
    EXPECT_FALSE(game.attackingCard.has_value());
    EXPECT_EQ(game.bFinished, false);
}

TEST(DurakTest, PlayerAttackPlayerDefend) {
    std::vector<std::unique_ptr<IActor>> actors;
    actors.emplace_back(new Player{0, true});
    actors.emplace_back(new Player{1, false});
    DurakGame game(actors);

    game.hands[0][5] = Card{Suit::Hearts, Rank::Six};
    game.hands[1][5] = Card{Suit::Hearts, Rank::Seven};
    game.executePlayerGameCommand(0, SelectCommand(6));
    game.executePlayerGameCommand(1, SelectCommand(6));
    EXPECT_EQ(game.attackingActor, 0);
    EXPECT_EQ(game.state, DurakState::AttackerThinks);
    EXPECT_EQ(game.table.size(), 2);
    EXPECT_FALSE(game.attackingCard.has_value());
    game.executePlayerGameCommand(0, EndCommand());
    EXPECT_EQ(game.hands[0].size(), 6);
    EXPECT_EQ(game.hands[1].size(), 6);
    EXPECT_EQ(game.attackingActor, 1);
    EXPECT_EQ(game.state, DurakState::AttackerThinks);
}

TEST(DurakTest, PlayerAttackBotDefend) {
    std::vector<std::unique_ptr<IActor>> actors;
    actors.emplace_back(new Player{0, true});
    actors.emplace_back(new Bot{std::make_unique<SortedStrategy>(0.0), 0});
    DurakGame game(actors);

    game.trump = Suit::Diamonds;
    game.hands[0][0] = Card{Suit::Diamonds, Rank::Ace};
    game.hands[0][5] = Card{Suit::Hearts, Rank::Ace};
    game.hands[1][5] = Card{Suit::Diamonds, Rank::Six};
    game.executePlayerGameCommand(0, SelectCommand(6));
    // bot calls executeGameCommand(), can defend

    EXPECT_EQ(game.attackingActor, 0);
    EXPECT_EQ(game.state, DurakState::AttackerThinks);
    EXPECT_EQ(game.table.size(), 2);
    game.executePlayerGameCommand(0, SelectCommand(1));
    // bot calls executeGameCommand(), has to take cards

    EXPECT_EQ(game.hands[0].size(), 6);
    EXPECT_EQ(game.hands[1].size(), 8);
    EXPECT_EQ(game.attackingActor, 0);
    EXPECT_EQ(game.state, DurakState::AttackerThinks);
}
