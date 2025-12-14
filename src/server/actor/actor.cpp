#include "actor.h"
#include "../player_manager.h"

void Player::freeFormNotify(const std::string& msg) {
        PlayerManager::sendToPlayer(id, msg);
}