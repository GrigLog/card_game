#include <sstream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <errno.h>
#include <algorithm>
#include <thread>
#include <chrono>
#include <iostream>
#include <map>
#include <unordered_map>
#include <memory>
#include "game_room.h"

class PlayerManager{
    std::unordered_map<unsigned, int> players;
    std::unordered_map<unsigned, std::string> playerIdToRoomId;
    std::unordered_map<std::string, std::unique_ptr<GameRoom>> rooms;

    int newPlayerPipeFd = -1;
    int nextPlayerId = 0;

    std::thread runThread;
    volatile bool running = true;

public:
    PlayerManager(int newPlayerPipeFd);
    ~PlayerManager();
    
    // Обработка команды от игрока
    void handleCommand(uint32_t playerId, const std::string& command);
    
    // Отправить ответ игроку
    void sendToPlayer(uint32_t playerId, const std::string& response);
private:
    void run();
};