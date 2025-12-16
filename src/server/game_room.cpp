#include "game_room.h"

#include <variant>

#include "actor/actor.h"
#include "common/common.h"
#include "player_manager.h"

TGameRoom::TGameRoom(const std::string& name, uint32_t ownerId, size_t maxPlayers)
    : Name(name)
    , OwnerId(ownerId)
    , MaxPlayers(maxPlayers) {
    PlayerIdToActorNum[ownerId] = Actors.size();
    Actors.emplace_back(new TPlayer{ownerId, true});
}

std::vector<const TGameRoom*> TGameRoom::GetAllRooms() {
    std::vector<const TGameRoom*> res;
    for (auto it = TGameRoom::AllRooms.begin(); it != TGameRoom::AllRooms.end();) {
        if (it->second.expired()) {
            it = TGameRoom::AllRooms.erase(it);
        } else {
            res.push_back(it->second.lock().get());
            it++;
        }
    }
    return res;
}

bool TGameRoom::AddPlayer(unsigned playerId) {
    // std::lock_guard<std::mutex> lock(playersMutex);
    if (IsFull()) {
        return false;
    }
    PlayerIdToActorNum[playerId] = Actors.size();
    Actors.emplace_back(new TPlayer(playerId, false));
    return true;
}

bool TGameRoom::AddBot(std::unique_ptr<IBotStrategy>&& strategy) {
    if (IsFull()) {
        return false;
    }
    Actors.emplace_back(new TBot(std::move(strategy), NextBotId++));
    return true;
}

bool TGameRoom::IsFull() const {
    // std::lock_guard<std::mutex> lock(playersMutex);
    return Actors.size() >= MaxPlayers;
}

void TGameRoom::NotifyPlayerLeft(unsigned playerId) {
    int num = PlayerIdToActorNum[playerId];
    std::string name = Actors[num]->GetName();
    for (const auto& [pid, num] : PlayerIdToActorNum) {
        if (pid != playerId) { // writing to an invalidated socket causes segfault
            TPlayerManager::SendToPlayer(pid, name + " left.");
        }
    }
    if (Started) {
        GameOpt.value().NotifyPlayerLeft(num);
    }
    Actors.erase(Actors.begin() + num);
    PlayerIdToActorNum.erase(playerId);
}

void TGameRoom::Start() {
    Started = true;
    GameOpt.emplace(Actors);
    for (int i = 0; i < Actors.size(); i++) {
        if (auto player = dynamic_cast<TPlayer*>(Actors[i].get())) {
            PlayerIdToActorNum[player->Id] = i;
        }
    }
}

std::string TGameRoom::HandleCommand(unsigned playerId, TSomeCommand cmd) {
    if (std::holds_alternative<TRoomCommand>(cmd)) {
        return ExecuteRoomCommand(playerId, std::get<TRoomCommand>(std::move(cmd)));
    }
    if (!Started) {
        return "error: This command only works in-game";
    }
    auto result = GameOpt.value().ExecutePlayerGameCommand(
        PlayerIdToActorNum[playerId], std::get<TGameCommand>(std::move(cmd)));
    // broadcast successful actions
    return (result.first ? "ok. " : "error: ") + result.second;
}

std::string TGameRoom::ExecuteRoomCommand(unsigned playerId, TRoomCommand cmd) {
    return std::visit(
        TVisitOverloadUtility{
            [&](TAddCommand c) -> std::string {
                if (Started) {
                    return "error: The game has already started";
                }
                if (IsFull()) {
                    return "error: No free player slots";
                }
                AddBot(std::move(c.Strategy));
                return "ok: bot added";
            },

            [&](TStartCommand c) -> std::string {
                if (OwnerId != playerId) {
                    return "error: You must be the owner to start the game";
                }
                if (Started) {
                    return "error: The game has already started";
                }
                if (Actors.size() < 2) {
                    return "error: Not enough players";
                }
                Start();
                return "ok.";
            }},
        std::move(cmd));
}
