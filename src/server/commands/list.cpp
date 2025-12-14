#include "list.h"
#include <sstream>
#include "../game_room.h"
#include "../actor/player.h"

std::string ListCommand::execute(
    unsigned playerId,
    std::unordered_map<unsigned, std::shared_ptr<GameRoom>>& playerToRoom
) {
    std::ostringstream oss;
    int i = 0;

    auto it = playerToRoom.find(playerId);
    if (it == playerToRoom.end()) {
        auto rooms = GameRoom::getAllRooms();
        for (auto room : rooms) {
            if (i++ > 0)
                oss << ",";
            oss << room->name;
        }
        return "Rooms: " + oss.str();
    } else {
        auto room = it->second.get();
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

