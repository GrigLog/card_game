#include "lobby_handler.h"
#include <sstream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <errno.h>
#include <algorithm>
#include <thread>
#include <chrono>
#include <iostream>

LobbyHandler::LobbyHandler(RoomManager& roomManager)
    : roomManager(roomManager), nextPlayerId(1), running(false) {
}

LobbyHandler::~LobbyHandler() {
    stop();
}

void LobbyHandler::start(int wakeupReadFd) {
    if (running) {
        return;
    }
    running = true;
    wakeupReadFd = wakeupReadFd;
    lobbyThread = std::thread(&LobbyHandler::run, this);
}

void LobbyHandler::stop() {
    if (!running) {
        return;
    }
    running = false;
    std::cout << "Stopping LobbyHandler..." << std::endl;
    if (lobbyThread.joinable()) {
        lobbyThread.join();
    }
    if (wakeupReadFd >= 0)
        close(wakeupReadFd);
    std::cout << "LobbyHandler stopped." << std::endl;
}

void LobbyHandler::run() {
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
        pollfds.push_back(pollfd{wakeupReadFd, POLLIN, 0});
        
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
                if (fd == wakeupReadFd) { // Присоединился новый игрок
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

void LobbyHandler::handleCommand(uint32_t playerId, const std::string& command) {
    std::istringstream iss(command);
    std::string cmd;
    iss >> cmd;
    
    // Поддержка сокращений
    if (cmd == "c") cmd = "create";
    else if (cmd == "j") cmd = "join";
    
    if (cmd == "create") {
        std::string name;
        size_t maxPlayers;
        if (iss >> name >> maxPlayers) {
            if (maxPlayers < 2 || maxPlayers > 6) {
                sendToPlayer(playerId, "error: Invalid number of players (2-6)");
                return;
            }
            
            if (roomManager.roomExists(name)) {
                sendToPlayer(playerId, "error: Room already exists");
                return;
            }
            
            if (roomManager.createRoom(name, playerId, maxPlayers)) {
                sendToPlayer(playerId, "ok: Room created: " + name);
            } else {
                sendToPlayer(playerId, "error: Failed to create room");
            }
        } else {
            sendToPlayer(playerId, "error: Usage: create <name> <max_players>");
        }
    } else if (cmd == "join") {
        std::string name;
        if (iss >> name) {
            GameRoom* room = roomManager.findRoom(name);
            if (!room) {
                sendToPlayer(playerId, "error: Room not found");
                return;
            }
            
            if (room->isFull()) {
                sendToPlayer(playerId, "error: Room is full");
                return;
            }
            
            // Получаем сокет игрока
            int socketFd = -1;
            auto it = players.find(playerId);
            if (it != players.end()) {
                socketFd = it->second;
                players.erase(it);
            }
            
            if (socketFd < 0) {
                sendToPlayer(playerId, "error: Player not found");
                return;
            }
            
            // Добавляем игрока в комнату
            if (room->addPlayer(playerId, socketFd)) {
                sendToPlayer(playerId, "ok: Joined room: " + name);
            } else {
                sendToPlayer(playerId, "error: Failed to join room");
                // Возвращаем сокет обратно в лобби
                players[playerId] = socketFd;
            }
        } else {
            sendToPlayer(playerId, "error: Usage: join <name>");
        }
    } else if (cmd == "list") {
        auto rooms = roomManager.listRooms();
        std::ostringstream oss;
        oss << "ok: Rooms: ";
        for (size_t i = 0; i < rooms.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << rooms[i];
        }
        sendToPlayer(playerId, oss.str());
    } else {
        sendToPlayer(playerId, "error: Unknown command. Use: create, join, list");
    }
}

void LobbyHandler::sendToPlayer(uint32_t playerId, const std::string& response) {
    auto it = players.find(playerId);
    if (it != players.end()) {
        Message::writeToSocket(it->second, response);
    }
}
