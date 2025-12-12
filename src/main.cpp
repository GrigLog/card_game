#include "card_game/GameServer.h"
#include "card_game/BotPlayer.h"
#include "card_game/BotStrategy.h"
#include <iostream>
#include <memory>
#include <thread>
#include <chrono>

int main() {
    using namespace card_game;
    
    std::cout << "=== Card Game Server ===" << std::endl;
    
    GameServer server;
    
    // Создаем ботов с разными стратегиями
    auto bot1 = std::make_shared<BotPlayer>(
        0, "Bot1", std::make_unique<RandomBotStrategy>());
    auto bot2 = std::make_shared<BotPlayer>(
        1, "Bot2", std::make_unique<LowCardBotStrategy>());
    auto bot3 = std::make_shared<BotPlayer>(
        2, "Bot3", std::make_unique<RandomBotStrategy>());
    
    server.addPlayer(bot1);
    server.addPlayer(bot2);
    server.addPlayer(bot3);
    
    server.start();
    
    // Создаем игровую сессию
    server.createSession({0, 1, 2});
    
    // Симулируем несколько ходов
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    // Боты делают ходы автоматически через свои стратегии
    for (int i = 0; i < 10; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        
        // Получаем текущего игрока и его действие
        // В реальной реализации это будет через сеть
    }
    
    std::cout << "Game simulation completed." << std::endl;
    
    server.stop();
    
    return 0;
}

