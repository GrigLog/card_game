#include <gtest/gtest.h>
#include "card_game/GameServer.h"
#include "card_game/HumanPlayer.h"
#include "card_game/BotPlayer.h"
#include "card_game/BotStrategy.h"
#include "card_game/Action.h"
#include <thread>
#include <chrono>

using namespace card_game;

TEST(GameServerTest, ServerCreation) {
    GameServer server;
    EXPECT_FALSE(server.isRunning());
}

TEST(GameServerTest, ServerStartStop) {
    GameServer server;
    
    server.start();
    EXPECT_TRUE(server.isRunning());
    
    server.stop();
    EXPECT_FALSE(server.isRunning());
}

TEST(GameServerTest, AddPlayers) {
    GameServer server;
    
    auto player1 = std::make_shared<HumanPlayer>(0, "Player1");
    auto player2 = std::make_shared<HumanPlayer>(1, "Player2");
    
    EXPECT_NO_THROW({
        server.addPlayer(player1);
        server.addPlayer(player2);
    });
}

TEST(GameServerTest, CreateSession) {
    GameServer server;
    server.start();
    
    auto player1 = std::make_shared<HumanPlayer>(0, "Player1");
    auto player2 = std::make_shared<HumanPlayer>(1, "Player2");
    
    server.addPlayer(player1);
    server.addPlayer(player2);
    
    EXPECT_NO_THROW({
        server.createSession({0, 1});
    });
    
    // Даем время на обработку
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    server.stop();
}

TEST(GameServerTest, HandlePlayerAction) {
    GameServer server;
    server.start();
    
    auto player1 = std::make_shared<HumanPlayer>(0, "Player1");
    auto player2 = std::make_shared<HumanPlayer>(1, "Player2");
    
    server.addPlayer(player1);
    server.addPlayer(player2);
    server.createSession({0, 1});
    
    // Даем время на старт игры
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    // Отправляем действие
    auto action = std::make_unique<PassAction>();
    EXPECT_NO_THROW({
        server.handlePlayerAction(0, std::move(action));
    });
    
    // Даем время на обработку
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    server.stop();
}

TEST(GameServerTest, MultipleSessions) {
    GameServer server;
    server.start();
    
    auto player1 = std::make_shared<HumanPlayer>(0, "Player1");
    auto player2 = std::make_shared<HumanPlayer>(1, "Player2");
    auto player3 = std::make_shared<HumanPlayer>(2, "Player3");
    auto player4 = std::make_shared<HumanPlayer>(3, "Player4");
    
    server.addPlayer(player1);
    server.addPlayer(player2);
    server.addPlayer(player3);
    server.addPlayer(player4);
    
    // Создаем две сессии
    server.createSession({0, 1});
    server.createSession({2, 3});
    
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    server.stop();
}

