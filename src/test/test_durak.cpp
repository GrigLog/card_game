#include <gtest/gtest.h>

#include <memory>
#include <vector>

#include "server/actor/actor.h"
#include "server/durak/durak.h"

TEST(DurakTest, PlayerAttackPlayerTake) {
    std::vector<std::unique_ptr<IActor>> actors;
    actors.emplace_back(new TPlayer{0, true});
    actors.emplace_back(new TPlayer{1, false});
    // broadcasts won't fail because PlayerManager has a safety check in sendToPlayer
    TDurakGame game(actors);

    game.ExecutePlayerGameCommand(0, TSelectCommand(1));
    EXPECT_EQ(game.State, EDurakState::DefenderThinks);
    EXPECT_TRUE(game.AttackingCard.has_value());
    game.ExecutePlayerGameCommand(1, TTakeCommand());
    EXPECT_EQ(game.Hands[0].size(), 6);
    EXPECT_EQ(game.Hands[1].size(), 7);
    EXPECT_EQ(game.AttackingActor, 0);
    EXPECT_EQ(game.State, EDurakState::AttackerThinks);
    EXPECT_FALSE(game.AttackingCard.has_value());
    EXPECT_EQ(game.Finished, false);
}

TEST(DurakTest, PlayerAttackPlayerDefend) {
    std::vector<std::unique_ptr<IActor>> actors;
    actors.emplace_back(new TPlayer{0, true});
    actors.emplace_back(new TPlayer{1, false});
    TDurakGame game(actors);

    game.Hands[0][5] = TCard{ESuit::Hearts, ERank::Six};
    game.Hands[1][5] = TCard{ESuit::Hearts, ERank::Seven};
    game.ExecutePlayerGameCommand(0, TSelectCommand(6));
    game.ExecutePlayerGameCommand(1, TSelectCommand(6));
    EXPECT_EQ(game.AttackingActor, 0);
    EXPECT_EQ(game.State, EDurakState::AttackerThinks);
    EXPECT_EQ(game.Table.size(), 2);
    EXPECT_FALSE(game.AttackingCard.has_value());
    game.ExecutePlayerGameCommand(0, TEndCommand());
    EXPECT_EQ(game.Hands[0].size(), 6);
    EXPECT_EQ(game.Hands[1].size(), 6);
    EXPECT_EQ(game.AttackingActor, 1);
    EXPECT_EQ(game.State, EDurakState::AttackerThinks);
}

TEST(DurakTest, PlayerAttackBotDefend) {
    std::vector<std::unique_ptr<IActor>> actors;
    actors.emplace_back(new TPlayer{0, true});
    actors.emplace_back(new TBot{std::make_unique<TSortedStrategy>(0.0), 0});
    TDurakGame game(actors);

    game.Trump = ESuit::Diamonds;
    game.Hands[0][0] = TCard{ESuit::Diamonds, ERank::Ace};
    game.Hands[0][5] = TCard{ESuit::Hearts, ERank::Ace};
    game.Hands[1][5] = TCard{ESuit::Diamonds, ERank::Six};
    game.ExecutePlayerGameCommand(0, TSelectCommand(6));
    // bot calls executeGameCommand(), can defend

    EXPECT_EQ(game.AttackingActor, 0);
    EXPECT_EQ(game.State, EDurakState::AttackerThinks);
    EXPECT_EQ(game.Table.size(), 2);
    game.ExecutePlayerGameCommand(0, TSelectCommand(1));
    // bot calls executeGameCommand(), has to take cards

    EXPECT_EQ(game.Hands[0].size(), 6);
    EXPECT_EQ(game.Hands[1].size(), 8);
    EXPECT_EQ(game.AttackingActor, 0);
    EXPECT_EQ(game.State, EDurakState::AttackerThinks);
}
