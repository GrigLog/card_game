#include "room_manager.h"

bool RoomManager::createRoom(const std::string& name, uint32_t ownerId, size_t maxPlayers) {
    std::lock_guard<std::mutex> lock(roomsMutex);
    
    if (rooms.find(name) != rooms.end()) {
        return false; // Комната уже существует
    }
    
    rooms[name] = std::make_unique<GameRoom>(name, ownerId, maxPlayers);
    return true;
}

GameRoom* RoomManager::findRoom(const std::string& name) {
    std::lock_guard<std::mutex> lock(roomsMutex);
    auto it = rooms.find(name);
    if (it != rooms.end()) {
        return it->second.get();
    }
    return nullptr;
}

void RoomManager::removeRoom(const std::string& name) {
    std::lock_guard<std::mutex> lock(roomsMutex);
    rooms.erase(name);
}

std::vector<std::string> RoomManager::listRooms() const {
    std::lock_guard<std::mutex> lock(roomsMutex);
    std::vector<std::string> result;
    result.reserve(rooms.size());
    for (const auto& [name, room] : rooms) {
        result.push_back(name);
    }
    return result;
}

bool RoomManager::roomExists(const std::string& name) const {
    std::lock_guard<std::mutex> lock(roomsMutex);
    return rooms.find(name) != rooms.end();
}
