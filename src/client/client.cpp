#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "../common/common.h"
#include "../common/message.h"
#include <unistd.h>

using namespace std;

int main(int argc, char** argv) {
    cout << "Client is starting..." << endl;
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Failed to create a socket");
        exit(1);
    }
    in_addr_t server_address = INADDR_LOOPBACK; //localhost for now
    sockaddr_in address{AF_INET, htons(PORT), htonl(server_address)};
    if (connect(sockfd, reinterpret_cast<sockaddr*>(&address), sizeof(address)) == -1) {
        perror(format("Failed to connect to {}:{}", server_address, PORT).c_str());
        exit(1);
    }
    cout << "Connected to game server." << endl;
    
    string line;
    while (getline(cin, line)) {
        if (line.empty()) {
            continue;
        }
        
        // Отправляем команду на сервер
        if (!Message::writeToSocket(sockfd, line)) {
            cerr << "Failed to send command" << endl;
            break;
        }
        
        // Читаем ответ от сервера
        string response;
        if (!Message::readFromSocket(sockfd, response)) {
            cerr << "Connection closed or error reading response" << endl;
            break;
        }
        
        cout << response << endl;
    }
    
    close(sockfd);
    return 0;
}
