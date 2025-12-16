#include <iostream>
#include <netinet/in.h>
#include <string>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "../common/common.h"
#include "../common/message.h"

using namespace std;

int main(int argc, char** argv) {
    cout << "Client is starting..." << endl;
    int sockFd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockFd == -1) {
        perror("Failed to create a socket");
        exit(1);
    }
    in_addr_t serverAddress = INADDR_LOOPBACK; // localhost for now
    sockaddr_in address{AF_INET, htons(PORT), htonl(serverAddress)};
    if (connect(sockFd, reinterpret_cast<sockaddr*>(&address), sizeof(address)) ==
        -1) {
        perror(format("Failed to connect to {}:{}", serverAddress, PORT).c_str());
        exit(1);
    }
    cout << "Connected to game server." << endl;

    // Настраиваем poll для двух файловых дескрипторов:
    // 1. STDIN_FILENO (0) - консольный ввод
    // 2. sockfd - сокет для сообщений от сервера
    pollfd fds[2] = {{STDIN_FILENO, POLLIN, 0}, {sockFd, POLLIN, 0}};

    string inputLine;
    bool running = true;

    while (running) {
        int pollCount = poll(fds, 2, -1);
        if (pollCount == -1) {
            perror("poll failed");
            break;
        }

        // 1. Проверяем, есть ли ввод с консоли (stdin)
        if (fds[0].revents & POLLIN) {
            if (!getline(cin, inputLine)) {
                // EOF (Ctrl+D) или ошибка чтения
                cout << "End of input. Closing connection." << endl;
                break;
            }

            if (!inputLine.empty()) {
                // Отправляем команду на сервер
                if (!TMessage::WriteToSocket(sockFd, inputLine)) {
                    cerr << "Failed to send command" << endl;
                    break;
                }
            }
        }

        // 2. Проверяем, есть ли данные от сервера
        if (fds[1].revents & POLLIN) {
            string response;
            if (!TMessage::ReadFromSocket(sockFd, response)) {
                cerr << "Connection closed or error reading from server" << endl;
                break;
            }

            // Выводим ответ от сервера
            if (!response.empty()) {
                cout << response << endl;
            }
        }

        // 3. Проверяем ошибки на сокете
        if (fds[1].revents & (POLLERR | POLLHUP | POLLNVAL)) {
            cerr << "Socket error or connection closed" << endl;
            break;
        }
    }

    close(sockFd);
    return 0;
}
