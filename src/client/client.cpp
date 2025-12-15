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
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Failed to create a socket");
        exit(1);
    }
    in_addr_t server_address = INADDR_LOOPBACK; // localhost for now
    sockaddr_in address{AF_INET, htons(PORT), htonl(server_address)};
    if (connect(sockfd, reinterpret_cast<sockaddr*>(&address), sizeof(address)) ==
        -1) {
        perror(format("Failed to connect to {}:{}", server_address, PORT).c_str());
        exit(1);
    }
    cout << "Connected to game server." << endl;

    // Настраиваем poll для двух файловых дескрипторов:
    // 1. STDIN_FILENO (0) - консольный ввод
    // 2. sockfd - сокет для сообщений от сервера
    pollfd fds[2] = {{STDIN_FILENO, POLLIN, 0}, {sockfd, POLLIN, 0}};

    string input_line;
    bool running = true;

    while (running) {
        int poll_count = poll(fds, 2, -1);
        if (poll_count == -1) {
            perror("poll failed");
            break;
        }

        // 1. Проверяем, есть ли ввод с консоли (stdin)
        if (fds[0].revents & POLLIN) {
            if (!getline(cin, input_line)) {
                // EOF (Ctrl+D) или ошибка чтения
                cout << "End of input. Closing connection." << endl;
                break;
            }

            if (!input_line.empty()) {
                // Отправляем команду на сервер
                if (!Message::writeToSocket(sockfd, input_line)) {
                    cerr << "Failed to send command" << endl;
                    break;
                }
            }
        }

        // 2. Проверяем, есть ли данные от сервера
        if (fds[1].revents & POLLIN) {
            string response;
            if (!Message::readFromSocket(sockfd, response)) {
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

    close(sockfd);
    return 0;
}
