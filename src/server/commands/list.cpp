#include "list.h"
#include <sstream>
#include "../game_room.h"
#include "../actor/player.h"

std::string ListCommand::execute(
    unsigned playerId,
    std::unordered_map<unsigned, std::string>& playerIdToRoomId,
    std::unordered_map<std::string, std::unique_ptr<GameRoom>>& rooms
) {
    std::ostringstream oss;
    int i = 0;

    auto it = playerIdToRoomId.find(playerId);
    if (it == playerIdToRoomId.end()) {
        for (const auto& [name, room] : rooms) {
            if (i++ > 0)
                oss << ",";
            oss << name;
        }
        return "Rooms: " + oss.str();
    } else {
        auto name = it->second;
        auto room = rooms[name].get();
        for (const auto& actor : room->actors) {
            if (i++ > 0)
                oss << ",";
            oss << actor->getName();
            if (actor->isOwner()) {
                oss << "(owner)";
            }
        }
        return "Room members: " + oss.str();
    }
}

