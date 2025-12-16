#pragma once
#include <memory>

#include "../common/common.h"
#include "accept_handler.h"
#include "pipe_wrapper.h"
#include "player_manager.h"

// RAII класс с основной логикой сервера
class TGameServer {
public:
    TPipeWrapper NewPlayerPipe;
    TAcceptHandler AcceptHandler;
    TPlayerManager PlayerManager;

    TGameServer();
    ~TGameServer();
};
