#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include <winsock2.h>
#include <string>
#include <vector>
#pragma comment(lib,"ws2_32.lib")

#define PORT 65000
#define PACKET_SIZE 1024

std::vector<SOCKADDR_IN> memberList;
SOCKADDR_IN memList[100] = { NULL, };

bool operator==(const SOCKADDR_IN& a, const SOCKADDR_IN& b) {
    return (a.sin_family == b.sin_family &&
        a.sin_port == b.sin_port &&
        a.sin_addr.s_addr == b.sin_addr.s_addr);
}

void handleREG(std::string received, SOCKADDR_IN clientAddress) {
    if (received == "#REG") {
        auto findIdx = std::find(memberList.begin(), memberList.end(), clientAddress);
        if (findIdx == memberList.end()) {
            memberList.push_back(clientAddress);
            std::cout << "> Client Registered. IP : " << inet_ntoa(clientAddress.sin_addr) << ", " << "PORT : " << clientAddress.sin_port << std::endl;
        }
    }
    else if (received == "#DEREG" || received == "exit") {
        auto removeIdx = std::find(memberList.begin(), memberList.end(), clientAddress);
        if (removeIdx != memberList.end()) {
            std::cout << "> Client Deregistered. IP : " << inet_ntoa(clientAddress.sin_addr) << ", " << "PORT : " << clientAddress.sin_port << std::endl;
            memberList.erase(removeIdx);
        }
    }
}

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
        if (received[0] == '#' || strcmp(received, "exit") == 0) {
            if (strcmp(received, "exit") == 0) {
                sendto(serverSocket, received, length, 0, (SOCKADDR*)&clientAddress, clientAddressSize);
            }
            handleREG(received, clientAddress);
        }
        else {
            if (memberList.size() > 0) {
                for (SOCKADDR_IN ele : memberList) {
                    sendto(serverSocket, received, length, 0, (SOCKADDR*)&ele, sizeof(ele));
                }
                std::cout << "Received ( " << received << " ) and echoed to " << memberList.size() << " clients." << std::endl;
            }
            else {
                std::cout << "no clients to echoing." << std::endl;
            }
        }
    }

    closesocket(serverSocket);
    WSACleanup();
    return 0;
}