#pragma once
#include <memory>

#include "../common/common.h"
#include "accept_handler.h"
#include "pipe_wrapper.h"
#include "player_manager.h"

// RAII класс с основной логикой сервера
struct GameServer {
    PipeWrapper newPlayerPipe;
    AcceptHandler acceptHandler;
    PlayerManager playerManager;

    GameServer();
    ~GameServer();
};
