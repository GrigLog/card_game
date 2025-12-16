#include "common/message.h"

#include <arpa/inet.h>
#include <cstring>
#include <sys/socket.h>
#include <unistd.h>

bool TMessage::ReadFromSocket(int socketFd, std::string& data) {
    // Читаем длину сообщения (4 байта)
    uint32_t length;
    ssize_t bytesRead = recv(socketFd, &length, sizeof(length), MSG_WAITALL);

    if (bytesRead == 0) {
        // Соединение закрыто
        return false;
    }
    if (bytesRead < 0) {
        // Ошибка чтения
        return false;
    }
    if (bytesRead != sizeof(length)) {
        // Неполное чтение
        return false;
    }

    // Преобразуем из network byte order
    length = ntohl(length);

    if (length == 0 || length > 1024 * 1024) { // Максимум 1MB
        return false;
    }

    // Читаем данные
    data.resize(length);
    bytesRead = recv(socketFd, data.data(), length, MSG_WAITALL);

    if (bytesRead != static_cast<ssize_t>(length)) {
        return false;
    }

    return true;
}

bool TMessage::WriteToSocket(int socketFd, const std::string& data) {
    if (data.empty() || data.size() > 1024 * 1024) {
        return false;
    }

    // Преобразуем длину в network byte order
    uint32_t length = htonl(static_cast<uint32_t>(data.size()));

    // Отправляем длину
    ssize_t bytesSent = send(socketFd, &length, sizeof(length), 0);
    if (bytesSent != sizeof(length)) {
        return false;
    }

    // Отправляем данные
    bytesSent = send(socketFd, data.data(), data.size(), 0);
    if (bytesSent != static_cast<ssize_t>(data.size())) {
        return false;
    }

    return true;
}
