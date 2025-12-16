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

class TPlayerManager {
    inline static std::unordered_map<unsigned, int>
        Players; // todo: singleton, idk
    std::unordered_map<unsigned, std::shared_ptr<TGameRoom>> PlayerRooms;

    int NewPlayerPipeFd;
    int NextPlayerId;

    std::thread RunThread;
    volatile bool Running;

public:
    TPlayerManager(int newPlayerPipeFd);
    ~TPlayerManager();

    static void SendToPlayer(uint32_t playerId, const std::string& response);

    void DestroyRoom(std::shared_ptr<TGameRoom> room, const std::string& msg);

    // Обработка команды от игрока
    std::string HandleCommand(uint32_t playerId, TSomeCommand cmd);
    std::string ExecuteLobbyCommand(unsigned playerId, TLobbyCommand cmd);

private:
    void run();
};