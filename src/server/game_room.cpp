#include "game_room.h"

GameRoom::GameRoom(const std::string& name, uint32_t ownerId, size_t maxPlayers)
    : name(name), ownerId(ownerId), maxPlayers(maxPlayers) {
}

bool GameRoom::addPlayer(uint32_t playerId, int socketFd) {
    std::lock_guard<std::mutex> lock(playersMutex);
    if (players.size() >= maxPlayers) {
        return false;
    }
    players.push_back({playerId, socketFd});
    return true;
}

bool GameRoom::isFull() const {
    std::lock_guard<std::mutex> lock(playersMutex);
    return players.size() >= maxPlayers;
}

size_t GameRoom::playerCount() const {
    std::lock_guard<std::mutex> lock(playersMutex);
    return players.size();
}

std::vector<uint32_t> GameRoom::getPlayerIds() const {
    std::lock_guard<std::mutex> lock(playersMutex);
    std::vector<uint32_t> ids;
    ids.reserve(players.size());
    for (const auto& player : players) {
        ids.push_back(player.id);
    }
    return ids;
}
