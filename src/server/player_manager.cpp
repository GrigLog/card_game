#include "player_manager.h"
#include <sys/poll.h>
#include "message.h"
#include "commands/command.h"
#include <memory>
#include <sstream>

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
    auto cmd = parseCommand(command);
    
    if (!cmd) {
        sendToPlayer(playerId, "error: Unknown command. Use: create, join, list");
        return;
    }
    
    std::string response = cmd->execute(static_cast<unsigned>(playerId), playerIdToRoomId, rooms);
    sendToPlayer(playerId, response);
}

void PlayerManager::sendToPlayer(uint32_t playerId, const std::string& response) {
    auto it = players.find(playerId);
    if (it != players.end()) {
        Message::writeToSocket(it->second, response);
    }
}
