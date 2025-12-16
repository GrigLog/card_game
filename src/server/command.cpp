#include "command.h"

#include <sstream>

#include "actor/strategy.h"

std::optional<TSomeCommand> parseCommand(const std::string& commandStr) {
    std::istringstream iss(commandStr);
    std::string cmd;
    iss >> cmd;

    if (cmd.empty()) {
        return {};
    }
    auto it = TCommand::FIRST_LETTERS.find(cmd[0]);
    if (it == TCommand::FIRST_LETTERS.end()) {
        try {
            int num = std::stoi(cmd);
            return std::optional{TSelectCommand(num)}; // todo: remove std::optional?
        } catch (...) {
        }
        return {};
    }
    TCommand::EType type = it->second;
    const std::string& fullCommand =
        TCommand::KNOWN_COMMANDS[static_cast<int>(type)];
    if (fullCommand.size() < cmd.size() ||
        fullCommand.substr(0, cmd.size()) != cmd) {
        return {};
    }

    switch (type) {
        case TCommand::EType::Create: {
            std::string name;
            size_t maxPlayers;
            if (iss >> name >> maxPlayers) {
                return std::optional{TCreateCommand(name, maxPlayers)};
            }
            return {};
        }
        case TCommand::EType::Join: {
            std::string name;
            if (iss >> name) {
                return std::optional{TJoinCommand(name)};
            }
            return {};
        }
        case TCommand::EType::List: {
            return std::optional{TListCommand()};
        }
        case TCommand::EType::Add: {
            if (auto strat = IBotStrategy::Parse(iss)) {
                return std::optional{TAddCommand(std::move(strat))};
            }
            return {};
        }
        case TCommand::EType::Start: {
            return std::optional{TStartCommand()};
        }
        case TCommand::EType::Finish: {
            return std::optional{TFinishCommand()};
        }
        case TCommand::EType::Take: {
            return std::optional{TTakeCommand()};
        }
        case TCommand::EType::End: {
            return std::optional{TEndCommand()};
        }
        default:
            return {};
    }
}
