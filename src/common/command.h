#pragma once

#include <string>
#include <variant>
#include <optional>

struct CreateCommand {
    std::string name;
    size_t maxPlayers;
};

struct JoinCommand {
    std::string name;
};

struct ListCommand {
};

// Тип команды - variant всех возможных команд
using Command = std::variant<CreateCommand, JoinCommand, ListCommand>;

// Парсинг команды из строки
// Возвращает optional с Command, или nullopt если команда не распознана
std::optional<Command> parseCommand(const std::string& commandStr);

