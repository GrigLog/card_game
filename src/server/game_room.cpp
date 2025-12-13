#include "game_room.h"

GameRoom::GameRoom(const std::string& name, uint32_t ownerId, size_t maxPlayers)
    : name(name), ownerId(ownerId), maxPlayers(maxPlayers) {
}

bool GameRoom::addPlayer(unsigned playerId) {
    std::lock_guard<std::mutex> lock(playersMutex);
    if (players.size() >= maxPlayers) {
        return false;
    }
    players.push_back(playerId);
    return true;
}

bool GameRoom::isFull() const {
    std::lock_guard<std::mutex> lock(playersMutex);
    return players.size() >= maxPlayers;
}
