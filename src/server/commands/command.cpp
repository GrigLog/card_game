#include "command.h"
#include "create.h"
#include "join.h"
#include "list.h"
#include <sstream>
#include "../actor/strategy.h"
#include "game/add.h"


std::unique_ptr<Command> parseCommand(const std::string& commandStr) {
    std::istringstream iss(commandStr);
    std::string cmd;
    iss >> cmd;

    if (cmd.empty())
        return nullptr;
    auto it = Command::FIRST_LETTERS.find(cmd[0]);
    if (it == Command::FIRST_LETTERS.end()) {
        try {
            int num = std::stoi(cmd);
            return nullptr; //todo: return SelectCommand
        } catch (...) {}
        return nullptr;
    }
    Command::Type type = it->second;
    const std::string& fullCommand = Command::KNOWN_COMMANDS[static_cast<int>(type)];
    if (fullCommand.size() < cmd.size() || fullCommand.substr(0, cmd.size()) != cmd)
        return nullptr;
    
    
    switch (type) {
    case Command::Type::Create: {
        std::string name;
        size_t maxPlayers;
        if (iss >> name >> maxPlayers) {
            return std::make_unique<CreateCommand>(name, maxPlayers);
        }
        return nullptr;
    } case Command::Type::Join: {
        std::string name;
        if (iss >> name) {
            return std::make_unique<JoinCommand>(name);
        }
        return nullptr;
    } case Command::Type::List: {
        return std::make_unique<ListCommand>();
    } case Command::Type::Add: {
        if (auto strat = IBotStrategy::parse(iss))
            return std::make_unique<AddCommand>(std::move(strat));
        return nullptr;
    } default:
        return nullptr;
    }
}

