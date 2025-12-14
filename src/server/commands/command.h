#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <vector>

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

    enum class Type : uint8_t {
        List = 0,
        Create,
        Join,

        Add,
        Start, 

        Select,
        Take,
        End
    };
    //NOTE: Must have the same order as Type declaration
    inline static const std::vector<std::string> KNOWN_COMMANDS = { 
        "list", "create", "join", 
        "add", "start", 
        "select", "take", "end"};
    //NOTE: start and select have the same first letters, but "select" is not actually typed
    inline static const std::unordered_map<char, Type> FIRST_LETTERS = {
        {'l', Type::List}, {'c', Type::Create}, {'j', Type::Join}, 
        {'a', Type::Add}, {'s', Type::Start},
        {'t', Type::Take}, {'e', Type::End}
    };
    // inline static const std::vector<Type> LOBBY_COMMANDS = {Type::List, Type::Create, Type::Join};
    // inline static const std::vector<Type> OWNER_COMMANDS = {Type::Add, Type::Start};
    // inline static const std::vector<Type> GAME_COMMANDS = {Type::Select, Type::Take, Type::End};

    inline static const std::string LOBBY_COMMANDS_STR = "list, create <room_name> <max_players>, join <room_name>";
    inline static const std::string OWNER_COMMANDS_STR = "list, add <random/sorted> [sorted coefficient, e.g. 0.1], start";
    inline static const std::string GAME_COMMANDS_STR = "select <card_num>, take, end";
};

// Парсинг команды из строки
// Возвращает unique_ptr с Command, или nullptr если команда не распознана
std::unique_ptr<Command> parseCommand(const std::string& commandStr);

