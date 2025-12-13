#include "player_manager.h"
#include <sys/poll.h>
#include "message.h"
#include <memory>

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
                    players[nextPlayerId++] = newSocketFd;
                } else {  // Старый игрок выполнил команду
                    uint32_t playerId = playerIds[i];
                    std::string data;
                    if (Message::readFromSocket(fd, data)) {
                        handleCommand(playerId, data);
                    } else {
                        // Ошибка чтения
                        std::cout << std::format(
                            "Failed to parse command ({}) from player {}. Disconnecting.",
                            data, playerId) << std::endl;
                        players.erase(playerId);
                        close(fd);
                    }
                }
            } else if (revents & (POLLERR | POLLHUP | POLLNVAL)) {  // Ошибка сокета
                uint32_t playerId = playerIds[i];
                players.erase(playerId);
                close(fd);
            }
        }
    }
}


void PlayerManager::handleCommand(uint32_t playerId, const std::string& command) {
    std::istringstream iss(command);
    std::string cmd;
    iss >> cmd;
    
    // Поддержка сокращений
    if (cmd == "c") cmd = "create";
    else if (cmd == "j") cmd = "join";
    else if (cmd == "l") cmd = "list";
    
    if (cmd == "create") {
        std::string name;
        size_t maxPlayers;
        if (iss >> name >> maxPlayers) {
            if (maxPlayers < 2 || maxPlayers > 6) {
                sendToPlayer(playerId, "error: Invalid number of players (2-6)");
                return;
            }
            
            if (rooms.find(name) != rooms.end()) {
                sendToPlayer(playerId, "error: Room already exists");
                return;
            }
            
            std::unique_ptr<GameRoom> room = std::make_unique<GameRoom>(name, playerId, maxPlayers);
            if (room.get()) {
                sendToPlayer(playerId, "ok: Room created: " + name);
                rooms[name] = std::move(room);
            } else {
                sendToPlayer(playerId, "error: Failed to create room");
            }
        } else {
            sendToPlayer(playerId, "error: Usage: create <name> <max_players>");
        }
    } else if (cmd == "join") {
        std::string name;
        if (iss >> name) {
            auto it = rooms.find(name);
            if (it == rooms.end()) {
                sendToPlayer(playerId, "error: Room not found");
                return;
            }
            
            if (it->second->isFull()) {
                sendToPlayer(playerId, "error: Room is full");
                return;
            }
            
            // Добавляем игрока в комнату
            if (it->second->addPlayer(playerId)) {
                sendToPlayer(playerId, "ok: Joined room: " + name);
            } else {
                sendToPlayer(playerId, "error: Failed to join room");
            }
        } else {
            sendToPlayer(playerId, "error: Usage: join <name>");
        }
    } else if (cmd == "list") {
        int i = 0;
        std::ostringstream oss;
        for (const auto& [name, room] : rooms) {
            if (i++ > 0)
                oss << ",";
            oss << name;
        }
        sendToPlayer(playerId, "Rooms: " + oss.str());
    } else {
        sendToPlayer(playerId, "error: Unknown command. Use: create, join, list");
    } //TODO: game-specific commands are also parsed here and redirected to their respective rooms
}

void PlayerManager::sendToPlayer(uint32_t playerId, const std::string& response) {
    auto it = players.find(playerId);
    if (it != players.end()) {
        Message::writeToSocket(it->second, response);
    }
}
