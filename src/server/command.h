#pragma once

#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

struct IBotStrategy;

struct ListCommand {};
struct CreateCommand {
    std::string name;
    size_t maxPlayers;
};
struct JoinCommand {
    std::string name;
};

struct AddCommand {
    std::unique_ptr<IBotStrategy> strategy;
};
struct StartCommand {};
struct FinishCommand {};

struct SelectCommand {
    unsigned cardNum;
};
struct TakeCommand {};
struct EndCommand {};

using LobbyCommand =
    std::variant<ListCommand, CreateCommand, JoinCommand, FinishCommand>;
using RoomCommand = std::variant<AddCommand, StartCommand>;
using GameCommand = std::variant<SelectCommand, TakeCommand, EndCommand>;

using SomeCommand = std::variant<LobbyCommand, RoomCommand, GameCommand>;

struct Command {
    enum class Type : uint8_t {
        List = 0,
        Create,
        Join,

        Add,
        Start,
        Finish,

        Select,
        Take,
        End
    };
    // NOTE: Must have the same order as Type declaration
    inline static const std::vector<std::string> KNOWN_COMMANDS = {
        "list", "create", "join", "add", "start",
        "finish", "select", "take", "end"};
    // NOTE: start and select have the same first letters, but "select" is not
    // actually typed
    inline static const std::unordered_map<char, Type> FIRST_LETTERS = {
        {'l', Type::List}, {'c', Type::Create}, {'j', Type::Join}, {'a', Type::Add}, {'s', Type::Start}, {'f', Type::Finish}, {'t', Type::Take}, {'e', Type::End}};

    inline static const std::string LOBBY_COMMANDS_STR =
        "list, create <room_name> <max_players>, join <room_name>";
    inline static const std::string OWNER_COMMANDS_STR =
        "list, add <random/sorted> [sorted coefficient, e.g. 0.1], start, finish";
    inline static const std::string GAME_COMMANDS_STR =
        "<selected card num>, take, end";
};

// Парсинг команды из строки
// Возвращает unique_ptr с Command, или nullptr если команда не распознана
std::optional<SomeCommand> parseCommand(const std::string& commandStr);
