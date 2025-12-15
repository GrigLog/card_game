#pragma once

#include <optional>
#include <string>

// Простой текстовый протокол сообщений
// Формат: длина сообщения (4 байта, network byte order) + данные (строка)

class Message {
public:
    // Чтение сообщения из сокета
    // Возвращает true если сообщение прочитано успешно, false при ошибке/закрытии
    static bool readFromSocket(int socketFd, std::string& data);

    // Запись сообщения в сокет
    // Возвращает true если запись успешна, false при ошибке
    static bool writeToSocket(int socketFd, const std::string& data);
};
