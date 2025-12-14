#include "command.h"
#include <sstream>
#include "actor/strategy.h"


std::optional<SomeCommand> parseCommand(const std::string& commandStr) {
    std::istringstream iss(commandStr);
    std::string cmd;
    iss >> cmd;

    if (cmd.empty())
        return {};
    auto it = Command::FIRST_LETTERS.find(cmd[0]);
    if (it == Command::FIRST_LETTERS.end()) {
        try {
            int num = std::stoi(cmd);
            return std::optional{SelectCommand(num)}; //todo: remove std::optional?
        } catch (...) {}
        return {};
    }
    Command::Type type = it->second;
    const std::string& fullCommand = Command::KNOWN_COMMANDS[static_cast<int>(type)];
    if (fullCommand.size() < cmd.size() || fullCommand.substr(0, cmd.size()) != cmd)
        return {};
    
    
    switch (type) {
    case Command::Type::Create: {
        std::string name;
        size_t maxPlayers;
        if (iss >> name >> maxPlayers) {
            return std::optional{CreateCommand(name, maxPlayers)};
        }
        return {};
    } case Command::Type::Join: {
        std::string name;
        if (iss >> name) {
            return std::optional{JoinCommand(name)};
        }
        return {};
    } case Command::Type::List: {
        return std::optional{ListCommand()};
    } case Command::Type::Add: {
        if (auto strat = IBotStrategy::parse(iss))
            return std::optional{AddCommand(std::move(strat))};
        return {};
    } case Command::Type::Start: {
        return std::optional{StartCommand()};
    } case Command::Type::Finish: {
        return std::optional{FinishCommand()};
    } case Command::Type::Take: {
        return std::optional{TakeCommand()};
    } case Command::Type::End: {
        return std::optional{FinishCommand()};
    }default:
        return {};
    }
}


