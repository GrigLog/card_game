#pragma once

#include <string>
#include <variant>
#include <optional>

struct CreateRoomCommand {
    std::string name;
    size_t maxPlayers;
};

struct JoinRoomCommand {
    std::string name;
};

struct ListRoomsCommand {
};

// Тип команды - variant всех возможных команд
using Command = std::variant<CreateRoomCommand, JoinRoomCommand, ListRoomsCommand>;

// Парсинг команды из строки
// Возвращает optional с Command, или nullopt если команда не распознана
std::optional<Command> parseCommand(const std::string& commandStr);

