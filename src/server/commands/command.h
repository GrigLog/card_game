#pragma once

#include <string>
#include <unordered_map>
#include <memory>

class GameRoom;

// Базовый класс для всех команд
struct Command {
    virtual ~Command() = default;
    
    // Выполнить команду
    // playerIdToRoomId - маппинг игрок -> комната
    // rooms - все комнаты
    // playerId - ID игрока, выполняющего команду
    // Возвращает ответное сообщение для игрока
    virtual std::string execute(
        unsigned playerId,
        std::unordered_map<unsigned, std::string>& playerIdToRoomId,
        std::unordered_map<std::string, std::unique_ptr<GameRoom>>& rooms
    ) = 0;
};

// Парсинг команды из строки
// Возвращает unique_ptr с Command, или nullptr если команда не распознана
std::unique_ptr<Command> parseCommand(const std::string& commandStr);

