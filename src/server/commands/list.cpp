#include "list.h"
#include <sstream>

std::string ListCommand::execute(
    unsigned playerId,
    std::unordered_map<unsigned, std::string>& playerIdToRoomId,
    std::unordered_map<std::string, std::unique_ptr<GameRoom>>& rooms
) {
    int i = 0;
    std::ostringstream oss;
    for (const auto& [name, room] : rooms) {
        if (i++ > 0)
            oss << ",";
        oss << name;
    }
    return "Rooms: " + oss.str();
}

