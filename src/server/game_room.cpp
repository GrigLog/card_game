#include "game_room.h"
#include "actor/player.h"

GameRoom::GameRoom(const std::string& name, uint32_t ownerId, size_t maxPlayers)
    : name(name), ownerId(ownerId), maxPlayers(maxPlayers) {
        actors.emplace_back(new Player{ownerId, true});
        //usedNames.emplace(name);
}

std::vector<const GameRoom*> GameRoom::getAllRooms() {
    std::vector<const GameRoom*> res;
    for (auto it = GameRoom::allRooms.begin(); it != GameRoom::allRooms.end(); ) {
        if (it->second.expired())
            it = GameRoom::allRooms.erase(it);
        else {
            res.push_back(it->second.lock().get());
            it++;
        }
    }
    return res;
}

bool GameRoom::addPlayer(unsigned playerId) {
    //std::lock_guard<std::mutex> lock(playersMutex);
    if (isFull()) {
        return false;
    }
    actors.emplace_back(new Player(playerId, false));
    return true;
}

bool GameRoom::addBot(std::unique_ptr<IBotStrategy>&& strategy) {
    if (isFull()) {
        return false;
    }
    actors.emplace_back(new Bot(std::move(strategy), nextBotId++));
    return true;
}

bool GameRoom::isFull() const {
    //std::lock_guard<std::mutex> lock(playersMutex);
    return actors.size() >= maxPlayers;
}

void GameRoom::notifyPlayerLeft(unsigned playerId) {
}
