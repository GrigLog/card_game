#pragma once
#include <algorithm>
#include <chrono>
#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>
#include <unordered_map>

#include "command.h"
#include "game_room.h"

class PlayerManager {
    inline static std::unordered_map<unsigned, int>
        players; // todo: singleton, idk
    std::unordered_map<unsigned, std::shared_ptr<GameRoom>> playerToRoom;

    int newPlayerPipeFd = -1;
    int nextPlayerId = 0;

    std::thread runThread;
    volatile bool running = true;

public:
    PlayerManager(int newPlayerPipeFd);
    ~PlayerManager();

    static void sendToPlayer(uint32_t playerId, const std::string& response);

    void destroyRoom(std::shared_ptr<GameRoom> room, const std::string& msg);

    // Обработка команды от игрока
    std::string handleCommand(uint32_t playerId, SomeCommand cmd);
    std::string executeLobbyCommand(unsigned playerId, LobbyCommand cmd);

private:
    void run();
};