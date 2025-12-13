#pragma once

#include "game_room.h"
#include <string>
#include <unordered_map>
#include <memory>
#include <mutex>

// Thread-safe менеджер комнат
class RoomManager {
    std::unordered_map<std::string, std::unique_ptr<GameRoom>> rooms;
    mutable std::mutex roomsMutex;
    
public:
    // Создать комнату
    // Возвращает true если комната создана, false если уже существует
    bool createRoom(const std::string& name, uint32_t ownerId, size_t maxPlayers);
    
    // Найти комнату по имени
    GameRoom* findRoom(const std::string& name);
    
    // Удалить комнату
    void removeRoom(const std::string& name);
    
    // Получить список всех комнат
    std::vector<std::string> listRooms() const;
    
    // Проверить, существует ли комната
    bool roomExists(const std::string& name) const;
};
