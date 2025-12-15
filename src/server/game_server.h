#pragma once
#include <memory>

#include "../common/common.h"
#include "accept_handler.h"
#include "pipe_wrapper.h"
#include "player_manager.h"

// RAII класс с основной логикой сервера
class GameServer {
    PipeWrapper newPlayerPipe;
    AcceptHandler acceptHandler;
    PlayerManager playerManager;

public:
    GameServer();
    ~GameServer();
};
