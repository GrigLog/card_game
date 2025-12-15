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

PlayerManager::PlayerManager(int newPlayerPipeFd)
    : newPlayerPipeFd(newPlayerPipeFd) {
    runThread = std::thread(&PlayerManager::run, this);
}

PlayerManager::~PlayerManager() {
    running = false;
    std::cout << "PlayerManager is stopping..." << std::endl;
    if (runThread.joinable()) {
        runThread.join();
    }
    std::cout << "PlayerManager stopped." << std::endl;
}

void PlayerManager::run() {
    while (running) {
        // Собираем сокеты для poll()
        std::vector<pollfd> pollfds;
        std::vector<uint32_t> playerIds;

        pollfds.reserve(players.size());
        playerIds.reserve(players.size());

        for (auto& [id, socketFd] : players) {
            pollfds.push_back(pollfd{socketFd, POLLIN, 0});
            playerIds.push_back(id);
        }
        pollfds.push_back(pollfd{newPlayerPipeFd, POLLIN, 0});

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
                if (fd == newPlayerPipeFd) { // Присоединился новый игрок
                    int newSocketFd;
                    read(fd, &newSocketFd, sizeof(newSocketFd));
                    int playerId = nextPlayerId++;
                    players[playerId] = newSocketFd;
                    sendToPlayer(
                        playerId,
                        "Welcome to Durak Online! Your name is Player" +
                            std::to_string(playerId) +
                            ".\nCommands available now: " + Command::LOBBY_COMMANDS_STR);
                    std::cout << "Welcome message sent." << std::endl;
                } else { // Старый игрок выполнил команду
                    uint32_t playerId = playerIds[i];
                    std::string data;
                    if (Message::readFromSocket(fd, data)) {
                        std::optional<SomeCommand> cmd_opt = parseCommand(data);
                        if (!cmd_opt) {
                            sendToPlayer(playerId, "error: Unknown command.");
                        } else {
                            sendToPlayer(playerId,
                                         handleCommand(playerId, std::move(cmd_opt.value())));
                        }

                    } else {
                        // Ошибка чтения
                        std::cout << std::format(
                                         "Failed to parse command ({}) from player {}. "
                                         "Disconnecting.",
                                         data, playerId)
                                  << std::endl;
                        if (auto it = playerToRoom.find(playerId);
                            it != playerToRoom.end()) {
                            // todo: don't destroy the room lol
                            it->second->notifyPlayerLeft(playerId);
                            playerToRoom.erase(it);
                            // auto room = it->second;
                            // it->second->playerIdToActorNum[playerId]
                            // destroyRoom(it->second.get(), "Someone has left the game. You
                            // have been sent back to lobby.");
                        }
                        players.erase(playerId);
                        close(fd);
                    }
                }
            } else if (revents & (POLLERR | POLLHUP | POLLNVAL)) { // Ошибка сокета
                uint32_t playerId = playerIds[i];
                players.erase(playerId);
                close(fd);
            }
        }
    }
}

void PlayerManager::sendToPlayer(uint32_t playerId,
                                 const std::string& response) {
    auto it = players.find(playerId);
    if (it != players.end()) {
        Message::writeToSocket(it->second, response);
    }
}

std::string PlayerManager::handleCommand(uint32_t playerId, SomeCommand cmd) {
    if (std::holds_alternative<LobbyCommand>(cmd)) {
        return executeLobbyCommand(playerId, std::get<LobbyCommand>(cmd));
    } else {
        auto it = playerToRoom.find(playerId);
        if (it == playerToRoom.end()) {
            return "error: Must be in a room to use this command";
        }
        return it->second->handleCommand(playerId, std::move(cmd));
    }
}

// You might not believe, but I originally had several Command classes with
// their own execute() methods, and it was LESS CONVENIENT
std::string PlayerManager::executeLobbyCommand(unsigned playerId,
                                               LobbyCommand cmd) {
    auto it = playerToRoom.find(playerId);
    bool hasRoom = it != playerToRoom.end();

    return std::visit(
        VisitOverloadUtility{
            [&](ListCommand c) -> std::string {
                std::ostringstream oss;
                int i = 0;

                if (!hasRoom) {
                    auto rooms = GameRoom::getAllRooms();
                    for (auto room : rooms) {
                        if (i++ > 0) {
                            oss << ",";
                        }
                        oss << room->name;
                    }
                    return "Rooms: " + oss.str();
                } else {
                    auto room = it->second.get();
                    for (const auto& actor : room->actors) {
                        if (i++ > 0) {
                            oss << ",";
                        }
                        oss << actor->getName();
                        if (actor->isOwner()) {
                            oss << "(owner)";
                        }
                    }
                    return "Room members: " + oss.str();
                }
            },

            [&](CreateCommand c) -> std::string {
                if (hasRoom) {
                    return "error: You are already in a room";
                }
                if (c.maxPlayers < 2 || c.maxPlayers > 6) {
                    return "error: Invalid number of players (2-6)";
                }
                if (GameRoom::allRooms.contains(c.name)) {
                    return "error: Room with this name already exists";
                }

                std::shared_ptr<GameRoom> room =
                    GameRoom::make(c.name, playerId, c.maxPlayers);
                if (room.get()) {
                    playerToRoom[playerId] = room;
                    return std::format(
                        "ok: Room '{}' created. You are the owner.\n"
                        "Start the game when there is enough players. Commands "
                        "available: {}",
                        c.name, Command::OWNER_COMMANDS_STR);
                } else {
                    return "error: Failed to create room";
                }
            },

            [&](JoinCommand c) -> std::string {
                if (hasRoom) {
                    return "error: You are already in a room";
                }
                auto it2 = GameRoom::allRooms.find(c.name);
                if (it2 == GameRoom::allRooms.end() || it2->second.expired()) {
                    return "error: Room not found";
                }
                auto room = it2->second.lock();
                if (room->isFull()) {
                    return "error: Room is full";
                }

                // Добавляем игрока в комнату
                if (room->addPlayer(playerId)) {
                    playerToRoom[playerId] = room;
                    return "ok: Joined room: " + c.name;
                } else {
                    return "error: Failed to join room";
                }
            },

            [&](FinishCommand c) -> std::string {
                if (!hasRoom) {
                    return "error: Must be in a room to use this command";
                }
                std::shared_ptr<GameRoom> room = it->second;
                if (room->ownerId != playerId) {
                    return "error: You must be the owner to close the room";
                }
                destroyRoom(room,
                            "Room owner has finished the game. You have been sent "
                            "back to lobby");
                return "ok: Room closed.";
            }

        },
        std::move(cmd));
}

void PlayerManager::destroyRoom(std::shared_ptr<GameRoom> room,
                                const std::string& msg) {
    for (const auto& actor : room->actors) {
        if (auto player = dynamic_cast<Player*>(actor.get())) {
            sendToPlayer(player->id, msg);
            playerToRoom.erase(player->id);
        }
    }
}
