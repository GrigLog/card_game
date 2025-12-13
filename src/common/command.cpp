#include "command.h"
#include <sstream>

std::optional<Command> parseCommand(const std::string& commandStr) {
    std::istringstream iss(commandStr);
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
            return CreateRoomCommand{name, maxPlayers};
        }
        return std::nullopt;
    } else if (cmd == "join") {
        std::string name;
        if (iss >> name) {
            return JoinRoomCommand{name};
        }
        return std::nullopt;
    } else if (cmd == "list") {
        return ListRoomsCommand{};
    }
    
    return std::nullopt;
}

