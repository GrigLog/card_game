#include "lobby_handler.h"
#include <sstream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <errno.h>
#include <algorithm>
#include <thread>
#include <chrono>

LobbyHandler::LobbyHandler(RoomManager& roomManager)
    : roomManager(roomManager), nextPlayerId(1), running(false) {
}

LobbyHandler::~LobbyHandler() {
    stop();
}

void LobbyHandler::start() {
    if (running) {
        return;
    }
    running = true;
    lobbyThread = std::thread(&LobbyHandler::run, this);
}

void LobbyHandler::stop() {
    if (!running) {
        return;
    }
    running = false;
    if (lobbyThread.joinable()) {
        lobbyThread.join();
    }
}

void LobbyHandler::addPlayer(int socketFd) {
    std::lock_guard<std::mutex> lock(newSocketsMutex);
    newSockets.push(socketFd);
}

void LobbyHandler::run() {
    while (running) {
        // Обрабатываем новые сокеты
        {
            std::lock_guard<std::mutex> lock(newSocketsMutex);
            while (!newSockets.empty()) {
                int socketFd = newSockets.front();
                newSockets.pop();
                
                // Устанавливаем non-blocking режим
                int flags = fcntl(socketFd, F_GETFL, 0);
                if (flags >= 0) {
                    fcntl(socketFd, F_SETFL, flags | O_NONBLOCK);
                }
                
                uint32_t playerId = nextPlayerId++;
                
                std::lock_guard<std::mutex> playersLock(playersMutex);
                players[playerId] = socketFd;
            }
        }
        
        // Собираем сокеты для poll()
        std::vector<struct pollfd> pollfds;
        std::vector<uint32_t> playerIds;
        
        {
            std::lock_guard<std::mutex> lock(playersMutex);
            pollfds.reserve(players.size());
            playerIds.reserve(players.size());
            
            for (auto& [id, socketFd] : players) {
                struct pollfd pfd;
                pfd.fd = socketFd;
                pfd.events = POLLIN;
                pfd.revents = 0;
                pollfds.push_back(pfd);
                playerIds.push_back(id);
            }
        }
        
        if (pollfds.empty()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }
        
        // Вызываем poll() с таймаутом 1 секунда
        int result = poll(pollfds.data(), pollfds.size(), 1000);
        
        if (result > 0) {
            // Обрабатываем готовые сокеты
            for (size_t i = 0; i < pollfds.size(); ++i) {
                if (pollfds[i].revents & POLLIN) {
                    uint32_t playerId = playerIds[i];
                    int socketFd = pollfds[i].fd;
                    
                    std::string data;
                    if (Message::readFromSocket(socketFd, data)) {
                        handleCommand(playerId, data);
                    } else {
                        // Ошибка чтения или соединение закрыто
                        std::lock_guard<std::mutex> lock(playersMutex);
                        players.erase(playerId);
                        close(socketFd);
                    }
                } else if (pollfds[i].revents & (POLLERR | POLLHUP | POLLNVAL)) {
                    // Ошибка сокета
                    uint32_t playerId = playerIds[i];
                    std::lock_guard<std::mutex> lock(playersMutex);
                    players.erase(playerId);
                    close(pollfds[i].fd);
                }
            }
        } else if (result < 0 && errno != EINTR) {
            // Ошибка poll
            break;
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
            {
                std::lock_guard<std::mutex> lock(playersMutex);
                auto it = players.find(playerId);
                if (it != players.end()) {
                    socketFd = it->second;
                    players.erase(it);
                }
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
                std::lock_guard<std::mutex> lock(playersMutex);
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
    std::lock_guard<std::mutex> lock(playersMutex);
    auto it = players.find(playerId);
    if (it != players.end()) {
        Message::writeToSocket(it->second, response);
    }
}
