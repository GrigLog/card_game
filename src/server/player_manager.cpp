#include "player_manager.h"

#include <memory>
#include <optional>
#include <sstream>
#include <sys/poll.h>
#include <variant>

#include "actor/actor.h"
#include "command.h"
#include "common/common.h"
#include "common/message.h"

TPlayerManager::TPlayerManager(int newPlayerPipeFd)
    : NewPlayerPipeFd(newPlayerPipeFd)
    , NextPlayerId(0)
    , Running(true)
{
    RunThread = std::thread(&TPlayerManager::run, this);
}

TPlayerManager::~TPlayerManager() {
    Running = false;
    std::cout << "PlayerManager is stopping..." << std::endl;
    if (RunThread.joinable()) {
        RunThread.join();
    }
    std::cout << "PlayerManager stopped." << std::endl;
}

void TPlayerManager::run() {
    while (Running) {
        // Собираем сокеты для poll()
        std::vector<pollfd> pollfds;
        std::vector<uint32_t> playerIds;

        pollfds.reserve(Players.size());
        playerIds.reserve(Players.size());

        for (auto& [id, socketFd] : Players) {
            pollfds.push_back(pollfd{socketFd, POLLIN, 0});
            playerIds.push_back(id);
        }
        pollfds.push_back(pollfd{NewPlayerPipeFd, POLLIN, 0});

        // Вызываем poll() с таймаутом 1 секунда (на всякий случай)
        int result = poll(pollfds.data(), pollfds.size(), 1000);
        if (result == -1) {
            perror("Lobby error on poll");
            exit(1);
        }
        // Обрабатываем новые данные
        for (size_t i = 0; i < pollfds.size(); ++i) {
            auto& [fd, _, revents] = pollfds[i];
            if (revents & POLLIN) {
                if (fd == NewPlayerPipeFd) { // Присоединился новый игрок
                    int newSocketFd;
                    read(fd, &newSocketFd, sizeof(newSocketFd));
                    int playerId = NextPlayerId++;
                    Players[playerId] = newSocketFd;
                    SendToPlayer(
                        playerId,
                        "Welcome to Durak Online! Your name is Player" +
                            std::to_string(playerId) +
                            ".\nCommands available now: " + TCommand::LOBBY_COMMANDS_STR);
                    std::cout << "Welcome message sent." << std::endl;
                } else { // Старый игрок выполнил команду
                    uint32_t playerId = playerIds[i];
                    std::string data;
                    if (TMessage::ReadFromSocket(fd, data)) {
                        std::optional<TSomeCommand> cmd_opt = parseCommand(data);
                        if (!cmd_opt) {
                            SendToPlayer(playerId, "error: Unknown command.");
                        } else {
                            SendToPlayer(playerId,
                                         HandleCommand(playerId, std::move(cmd_opt.value())));
                        }

                    } else {
                        // Ошибка чтения
                        std::cout << std::format(
                                         "Failed to parse command ({}) from player {}. "
                                         "Disconnecting.",
                                         data, playerId)
                                  << std::endl;
                        if (auto it = PlayerRooms.find(playerId);
                            it != PlayerRooms.end()) {
                            // todo: don't destroy the room lol
                            it->second->NotifyPlayerLeft(playerId);
                            PlayerRooms.erase(it);
                            // auto room = it->second;
                            // it->second->playerIdToActorNum[playerId]
                            // destroyRoom(it->second.get(), "Someone has left the game. You
                            // have been sent back to lobby.");
                        }
                        Players.erase(playerId);
                        close(fd);
                    }
                }
            } else if (revents & (POLLERR | POLLHUP | POLLNVAL)) { // Ошибка сокета
                uint32_t playerId = playerIds[i];
                Players.erase(playerId);
                close(fd);
            }
        }
    }
}

void TPlayerManager::SendToPlayer(uint32_t playerId,
                                 const std::string& response) {
    auto it = Players.find(playerId);
    if (it != Players.end()) {
        TMessage::WriteToSocket(it->second, response);
    }
}

std::string TPlayerManager::HandleCommand(uint32_t playerId, TSomeCommand cmd) {
    if (std::holds_alternative<TLobbyCommand>(cmd)) {
        return ExecuteLobbyCommand(playerId, std::get<TLobbyCommand>(cmd));
    } else {
        auto it = PlayerRooms.find(playerId);
        if (it == PlayerRooms.end()) {
            return "error: Must be in a room to use this command";
        }
        return it->second->HandleCommand(playerId, std::move(cmd));
    }
}

// You might not believe, but I originally had several Command classes with
// their own execute() methods, and it was LESS CONVENIENT
std::string TPlayerManager::ExecuteLobbyCommand(unsigned playerId,
                                               TLobbyCommand cmd) {
    auto it = PlayerRooms.find(playerId);
    bool hasRoom = it != PlayerRooms.end();

    return std::visit(
        TVisitOverloadUtility{
            [&](TListCommand c) -> std::string {
                std::ostringstream oss;
                int i = 0;

                if (!hasRoom) {
                    auto rooms = TGameRoom::GetAllRooms();
                    for (auto room : rooms) {
                        if (i++ > 0) {
                            oss << ",";
                        }
                        oss << room->Name;
                    }
                    return "Rooms: " + oss.str();
                } else {
                    auto room = it->second.get();
                    for (const auto& actor : room->Actors) {
                        if (i++ > 0) {
                            oss << ",";
                        }
                        oss << actor->GetName();
                        if (actor->IsOwner()) {
                            oss << "(owner)";
                        }
                    }
                    return "Room members: " + oss.str();
                }
            },

            [&](TCreateCommand c) -> std::string {
                if (hasRoom) {
                    return "error: You are already in a room";
                }
                if (c.MaxPlayers < 2 || c.MaxPlayers > 6) {
                    return "error: Invalid number of players (2-6)";
                }
                if (TGameRoom::AllRooms.contains(c.Name)) {
                    return "error: Room with this name already exists";
                }

                std::shared_ptr<TGameRoom> room =
                    TGameRoom::make(c.Name, playerId, c.MaxPlayers);
                if (room.get()) {
                    PlayerRooms[playerId] = room;
                    return std::format(
                        "ok: Room '{}' created. You are the owner.\n"
                        "Start the game when there is enough players. Commands "
                        "available: {}",
                        c.Name, TCommand::OWNER_COMMANDS_STR);
                } else {
                    return "error: Failed to create room";
                }
            },

            [&](TJoinCommand c) -> std::string {
                if (hasRoom) {
                    return "error: You are already in a room";
                }
                auto it2 = TGameRoom::AllRooms.find(c.Name);
                if (it2 == TGameRoom::AllRooms.end() || it2->second.expired()) {
                    return "error: Room not found";
                }
                auto room = it2->second.lock();
                if (room->IsFull()) {
                    return "error: Room is full";
                }

                // Добавляем игрока в комнату
                if (room->AddPlayer(playerId)) {
                    PlayerRooms[playerId] = room;
                    return "ok: Joined room: " + c.Name;
                } else {
                    return "error: Failed to join room";
                }
            },

            [&](TFinishCommand c) -> std::string {
                if (!hasRoom) {
                    return "error: Must be in a room to use this command";
                }
                std::shared_ptr<TGameRoom> room = it->second;
                if (room->OwnerId != playerId) {
                    return "error: You must be the owner to close the room";
                }
                DestroyRoom(room,
                            "Room owner has finished the game. You have been sent "
                            "back to lobby");
                return "ok: Room closed.";
            }

        },
        std::move(cmd));
}

void TPlayerManager::DestroyRoom(std::shared_ptr<TGameRoom> room,
                                const std::string& msg) {
    for (const auto& actor : room->Actors) {
        if (auto player = dynamic_cast<TPlayer*>(actor.get())) {
            SendToPlayer(player->Id, msg);
            PlayerRooms.erase(player->Id);
        }
    }
}
