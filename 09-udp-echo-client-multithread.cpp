#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include <winsock2.h>
#include <string>
#include <thread>
#pragma comment(lib,"ws2_32.lib")

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 65000
#define PACKET_SIZE 1024

SOCKET clientSocket;
SOCKADDR_IN serverAddress;
int serverAddressSize = sizeof(serverAddress);

void receiveMessages() {
    char received[PACKET_SIZE];
    int length;

    while (1) {
        length = recvfrom(clientSocket, received, PACKET_SIZE, 0, (SOCKADDR*)&serverAddress, &serverAddressSize);
        if (length > 0) {
            received[length] = NULL;
            if (strcmp(received, "exit") == 0) {
                break;
            }
            std::cout << "> Received: " << received << std::endl;
        }
    }
}

int main() {
    std::string sent;
    WSADATA wsa;

    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        std::cout << "WSAStartup failed" << std::endl;
        return 0;
    }

    clientSocket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(SERVER_PORT);
    serverAddress.sin_addr.s_addr = inet_addr(SERVER_IP);

    std::cout << "> Connect complete." << std::endl;

    std::thread recvThread(receiveMessages);

    while (1) {
        getline(std::cin, sent);
        if (sent == "") continue;
        sendto(clientSocket, sent.c_str(), sent.length(), 0, (SOCKADDR*)&serverAddress, serverAddressSize);
        if (sent == "exit") {
            std::cout << "> Disconnected." << std::endl;
            break;
        }
    }

    recvThread.join();
    closesocket(clientSocket);
    WSACleanup();
    return 0;
}