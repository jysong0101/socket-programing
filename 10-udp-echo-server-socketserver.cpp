#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include <winsock2.h>
#include <string>
#pragma comment(lib,"ws2_32.lib")

#define PORT 65000
#define PACKET_SIZE 1024

int main() {
    SOCKET serverSocket;
    SOCKADDR_IN serverAddress, clientAddress;
    char received[PACKET_SIZE];
    int clientAddressSize = sizeof(clientAddress);
    WSADATA wsa;

    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        std::cout << "> WSAStartup failed" << std::endl;
        return 0;
    }

    serverSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddress.sin_port = htons(PORT);

    bind(serverSocket, (SOCKADDR*)&serverAddress, sizeof(serverAddress));
    std::cout << "> Server is activated." << std::endl;

    while (1) {
        int length = recvfrom(serverSocket, received, PACKET_SIZE, 0, (SOCKADDR*)&clientAddress, &clientAddressSize);
        received[length] = NULL;
        std::cout << "> Received and Echoing to client: " << received << std::endl;
        sendto(serverSocket, received, length, 0, (SOCKADDR*)&clientAddress, clientAddressSize);
    }

    closesocket(serverSocket);
    WSACleanup();
    return 0;
}
