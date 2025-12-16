#pragma once

#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

struct IBotStrategy;

struct TListCommand {};
struct TCreateCommand {
    std::string Name;
    size_t MaxPlayers;
};
struct TJoinCommand {
    std::string Name;
};

struct TAddCommand {
    std::unique_ptr<IBotStrategy> Strategy;
};
struct TStartCommand {};
struct TFinishCommand {};

struct TSelectCommand {
    unsigned CardNum;
};
struct TTakeCommand {};
struct TEndCommand {};

using TLobbyCommand =
    std::variant<TListCommand, TCreateCommand, TJoinCommand, TFinishCommand>;
using TRoomCommand = std::variant<TAddCommand, TStartCommand>;
using TGameCommand = std::variant<TSelectCommand, TTakeCommand, TEndCommand>;

using TSomeCommand = std::variant<TLobbyCommand, TRoomCommand, TGameCommand>;

struct TCommand {
    enum class EType : uint8_t {
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
    inline static const std::unordered_map<char, EType> FIRST_LETTERS = {
        {'l', EType::List}, {'c', EType::Create}, {'j', EType::Join}, {'a', EType::Add}, {'s', EType::Start}, {'f', EType::Finish}, {'t', EType::Take}, {'e', EType::End}};

    inline static const std::string LOBBY_COMMANDS_STR =
        "list, create <room_name> <max_players>, join <room_name>";
    inline static const std::string OWNER_COMMANDS_STR =
        "list, add <random/sorted> [sorted coefficient, e.g. 0.1], start, finish";
    inline static const std::string GAME_COMMANDS_STR =
        "<selected card num>, take, end";
};

// Парсинг команды из строки
// Возвращает unique_ptr с Command, или nullptr если команда не распознана
std::optional<TSomeCommand> parseCommand(const std::string& commandStr);
