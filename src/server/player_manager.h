#pragma once
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
#include "command.h"

class PlayerManager{
    inline static std::unordered_map<unsigned, int> players; //todo: singleton, idk
    std::unordered_map<unsigned, std::shared_ptr<GameRoom>> playerToRoom;

    int newPlayerPipeFd = -1;
    int nextPlayerId = 0;

    std::thread runThread;
    volatile bool running = true;

public:
    PlayerManager(int newPlayerPipeFd);
    ~PlayerManager();
    
    static void sendToPlayer(uint32_t playerId, const std::string& response);

    void destroyRoom(GameRoom* room, const std::string& msg);

    // Обработка команды от игрока
    std::string handleCommand(uint32_t playerId, SomeCommand cmd);
    std::string executeLobbyCommand(unsigned playerId, LobbyCommand cmd);
private:
    void run();

    
};