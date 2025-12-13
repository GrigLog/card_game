#pragma once

#include "accept_handler.h"
#include "player_manager.h"
#include "../common/common.h"
#include <memory>
#include "pipe_wrapper.h"

// RAII класс с основной логикой сервера
class GameServer {
    PipeWrapper newPlayerPipe;
    AcceptHandler acceptHandler;
    PlayerManager playerManager;
public:
    GameServer();
    ~GameServer();
};
