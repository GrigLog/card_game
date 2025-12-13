#include "command.h"
#include "create.h"
#include "join.h"
#include "list.h"
#include <sstream>

std::unique_ptr<Command> parseCommand(const std::string& commandStr) {
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
            return std::make_unique<CreateCommand>(name, maxPlayers);
        }
        return nullptr;
    } else if (cmd == "join") {
        std::string name;
        if (iss >> name) {
            return std::make_unique<JoinCommand>(name);
        }
        return nullptr;
    } else if (cmd == "list") {
        return std::make_unique<ListCommand>();
    }
    
    return nullptr;
}

