#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "card_game/GameSession.h"
#include "card_game/HumanPlayer.h"
#include "card_game/Action.h"
#include "card_game/GameEvent.h"

using namespace card_game;
using ::testing::_;

// Mock-объект для тестирования
class MockPlayer : public IPlayer {
public:
    MOCK_METHOD(int, getId, (), (const, override));
    MOCK_METHOD(std::string, getName, (), (const, override));
    MOCK_METHOD(void, onEvent, (const GameEvent&), (override));
    MOCK_METHOD(void, receiveCards, (std::vector<Card>), (override));
    MOCK_METHOD(const std::vector<Card>&, getHand, (), (const, override));
    MOCK_METHOD(std::unique_ptr<Action>, makeMove, (const GameEvent&), (override));
    MOCK_METHOD(bool, canPlay, (), (const, override));
    
    // Хелпер для установки руки
    void setHand(std::vector<Card> hand) {
        hand_ = std::move(hand);
    }
    
private:
    std::vector<Card> hand_;
};

TEST(GameSessionTest, SessionCreation) {
    auto player1 = std::make_shared<HumanPlayer>(0, "Player1");
    auto player2 = std::make_shared<HumanPlayer>(1, "Player2");
    
    std::vector<std::shared_ptr<IPlayer>> players = {player1, player2};
    
    EXPECT_NO_THROW({
        GameSession session(players);
    });
}

TEST(GameSessionTest, SessionStart) {
    auto player1 = std::make_shared<HumanPlayer>(0, "Player1");
    auto player2 = std::make_shared<HumanPlayer>(1, "Player2");
    
    std::vector<std::shared_ptr<IPlayer>> players = {player1, player2};
    GameSession session(players);
    
    EXPECT_NO_THROW({
        session.start();
    });
    
    // После старта у игроков должны быть карты
    EXPECT_GT(player1->getHand().size(), 0);
    EXPECT_GT(player2->getHand().size(), 0);
}

TEST(GameSessionTest, ProcessPlayCardAction) {
    auto player1 = std::make_shared<HumanPlayer>(0, "Player1");
    auto player2 = std::make_shared<HumanPlayer>(1, "Player2");
    
    std::vector<std::shared_ptr<IPlayer>> players = {player1, player2};
    GameSession session(players);
    session.start();
    
    // Получаем карту из руки первого игрока
    auto hand = player1->getHand();
    if (!hand.empty()) {
        Card cardToPlay = hand[0];
        
        // Устанавливаем действие для игрока
        player1->setNextAction(std::make_unique<PlayCardAction>(cardToPlay));
        
        // Обрабатываем действие
        int currentPlayer = session.getCurrentPlayerId();
        if (currentPlayer == 0) {
            auto action = player1->makeMove(
                GameEvent(EventType::PlayerTurn, PlayerTurnEvent{0}));
            session.processAction(0, std::move(action));
            
            // Карта должна быть на столе
            auto tableCards = session.getTableCards();
            EXPECT_GT(tableCards.size(), 0);
        }
    }
}

TEST(GameSessionTest, ProcessPassAction) {
    auto player1 = std::make_shared<HumanPlayer>(0, "Player1");
    auto player2 = std::make_shared<HumanPlayer>(1, "Player2");
    
    std::vector<std::shared_ptr<IPlayer>> players = {player1, player2};
    GameSession session(players);
    session.start();
    
    int initialPlayer = session.getCurrentPlayerId();
    
    // Игрок делает пас
    player1->setNextAction(std::make_unique<PassAction>());
    
    if (initialPlayer == 0) {
        auto action = player1->makeMove(
            GameEvent(EventType::PlayerTurn, PlayerTurnEvent{0}));
        session.processAction(0, std::move(action));
        
        // Ход должен перейти к следующему игроку
        EXPECT_NE(session.getCurrentPlayerId(), initialPlayer);
    }
}

TEST(GameSessionTest, MockPlayerNotification) {
    auto mockPlayer = std::make_shared<MockPlayer>();
    
    ON_CALL(*mockPlayer, getId()).WillByDefault(::testing::Return(0));
    ON_CALL(*mockPlayer, getName()).WillByDefault(::testing::Return("MockPlayer"));
    ON_CALL(*mockPlayer, getHand()).WillByDefault(::testing::ReturnRef(std::vector<Card>()));
    ON_CALL(*mockPlayer, canPlay()).WillByDefault(::testing::Return(true));
    
    // Ожидаем, что игрок получит событие
    EXPECT_CALL(*mockPlayer, onEvent(_)).Times(::testing::AtLeast(1));
    
    std::vector<std::shared_ptr<IPlayer>> players = {mockPlayer};
    GameSession session(players);
    session.start();
}

TEST(GameSessionTest, GameOverCondition) {
    auto player1 = std::make_shared<HumanPlayer>(0, "Player1");
    auto player2 = std::make_shared<HumanPlayer>(1, "Player2");
    
    std::vector<std::shared_ptr<IPlayer>> players = {player1, player2};
    GameSession session(players);
    session.start();
    
    // Игра не должна быть завершена сразу после старта
    EXPECT_FALSE(session.isGameOver());
}

