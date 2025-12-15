#include "game_room.h"

#include <variant>

#include "actor/actor.h"
#include "common.h"
#include "player_manager.h"

GameRoom::GameRoom(const std::string& name, uint32_t ownerId, size_t maxPlayers)
    : name(name)
    , ownerId(ownerId)
    , maxPlayers(maxPlayers) {
    playerIdToActorNum[ownerId] = actors.size();
    actors.emplace_back(new Player{ownerId, true});
}

std::vector<const GameRoom*> GameRoom::getAllRooms() {
    std::vector<const GameRoom*> res;
    for (auto it = GameRoom::allRooms.begin(); it != GameRoom::allRooms.end();) {
        if (it->second.expired()) {
            it = GameRoom::allRooms.erase(it);
        } else {
            res.push_back(it->second.lock().get());
            it++;
        }
    }
    return res;
}

bool GameRoom::addPlayer(unsigned playerId) {
    // std::lock_guard<std::mutex> lock(playersMutex);
    if (isFull()) {
        return false;
    }
    playerIdToActorNum[playerId] = actors.size();
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
    // std::lock_guard<std::mutex> lock(playersMutex);
    return actors.size() >= maxPlayers;
}

void GameRoom::notifyPlayerLeft(unsigned playerId) {
    int num = playerIdToActorNum[playerId];
    std::string name = actors[num]->getName();
    for (const auto& [pid, num] : playerIdToActorNum) {
        if (pid != playerId) { // writing to an invalidated socket causes segfault
            PlayerManager::sendToPlayer(pid, name + " left.");
        }
    }
    if (bStarted) {
        gameOpt.value().notifyPlayerLeft(num);
    }
    actors.erase(actors.begin() + num);
    playerIdToActorNum.erase(playerId);
}

void GameRoom::start() {
    bStarted = true;
    gameOpt.emplace(actors);
    for (int i = 0; i < actors.size(); i++) {
        if (auto player = dynamic_cast<Player*>(actors[i].get())) {
            playerIdToActorNum[player->id] = i;
        }
    }
}

std::string GameRoom::handleCommand(unsigned playerId, SomeCommand cmd) {
    if (std::holds_alternative<RoomCommand>(cmd)) {
        return executeRoomCommand(playerId, std::get<RoomCommand>(std::move(cmd)));
    }
    if (!bStarted) {
        return "error: This comman only works in-game";
    }
    auto result = gameOpt.value().executePlayerGameCommand(
        playerIdToActorNum[playerId], std::get<GameCommand>(std::move(cmd)));
    // broadcast successful actions
    return (result.first ? "ok. " : "error: ") + result.second;
}

std::string GameRoom::executeRoomCommand(unsigned playerId, RoomCommand cmd) {
    return std::visit(
        VisitOverloadUtility{
            [&](AddCommand c) -> std::string {
                if (bStarted) {
                    return "error: The game has already started";
                }
                if (isFull()) {
                    return "error: No free player slots";
                }
                addBot(std::move(c.strategy));
                return "ok: bot added";
            },

            [&](StartCommand c) -> std::string {
                if (ownerId != playerId) {
                    return "error: You must be the owner to start the game";
                }
                if (bStarted) {
                    return "error: The game has already started";
                }
                if (actors.size() < 2) {
                    return "error: Not enough players";
                }
                start();
                return "ok.";
            }},
        std::move(cmd));
}
