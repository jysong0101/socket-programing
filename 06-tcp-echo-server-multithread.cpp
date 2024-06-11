#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include <winsock2.h>
#include <string>
#include <thread>
#include <vector>
#pragma comment(lib,"ws2_32.lib")

#define PORT 65000
#define PACKET_SIZE 1024

struct ClientInfo {
    SOCKET socket;
    sockaddr_in clientAddress;
    int threadNum;
};

HANDLE clientThreads[100] = { NULL };
ClientInfo clientPool[100];
int threadCnt = 0;

unsigned int __stdcall HandleClient(void* data);

SOCKET serverSocket;
SOCKADDR_IN serverAddress;

void client_accept() {
    while (1) {
        SOCKADDR_IN clientAddress = {};
        int clientSize = sizeof(clientAddress);

        SOCKET clientSocket = accept(serverSocket, (SOCKADDR*)&clientAddress, &clientSize);

        if (clientSocket == INVALID_SOCKET) {
            if (WSAGetLastError() == WSAEINTR) {
                break;
            }
            std::cout << "accept failed with error: " << WSAGetLastError() << std::endl;
            continue;
        }

        std::cout << "> Client connected by IP address " << inet_ntoa(clientAddress.sin_addr)
            << " with Port number " << ntohs(clientAddress.sin_port) << std::endl;

        ClientInfo* clientData = new ClientInfo;
        clientData->socket = clientSocket;
        clientData->clientAddress = clientAddress;

        HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, HandleClient, clientData, 0, NULL);

        for (int i = 2; i < 100; i++) {
            if (clientThreads[i] == NULL) {
                clientThreads[i] = hThread;
                threadCnt++;
                clientData->threadNum = i;
                clientPool[i] = *clientData;
                break;
            }
        }
    }
}

int main() {
    WSADATA wsa;

    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        std::cout << "> WSAStartup failed" << std::endl;
        return 0;
    }

    serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddress.sin_port = htons(PORT);

    bind(serverSocket, (SOCKADDR*)&serverAddress, sizeof(serverAddress));

    std::cout << "> Server is activated." << std::endl;

    listen(serverSocket, SOMAXCONN);

    std::thread client_listen(client_accept);

    char msg[PACKET_SIZE] = { 0 };
    while (1) {
        ZeroMemory(&msg, PACKET_SIZE);
        std::cin >> msg;
        if ((std::string)msg == "exit") {
            if (threadCnt > 0) {
                std::cout << "> Active threads are remained : " << threadCnt << " threads" << std::endl;
            }
            else {
                break;
            }
        }
        else continue;
    }

    closesocket(serverSocket);

    client_listen.join();
    for (HANDLE hThread : clientThreads) {
        if (hThread != NULL) {
            WaitForSingleObject(hThread, INFINITE);
            CloseHandle(hThread);
        }
    }


    WSACleanup();
    return 0;
}

unsigned int __stdcall HandleClient(void* data) {
    ClientInfo* clientData = static_cast<ClientInfo*>(data);
    SOCKET clientSocket = clientData->socket;
    sockaddr_in clientAddr = clientData->clientAddress;
    int threadN = clientData->threadNum;
    delete clientData;

    char received[PACKET_SIZE];
    int recvSize;

    while (1) {
        ZeroMemory(&received, PACKET_SIZE);
        recvSize = recv(clientSocket, received, sizeof(received), 0);
        if (recvSize > 0) {
            std::cout << "> Received and Echoing to client : " << received << " (by Thread - " << threadN << ")" << std::endl;
            if (strcmp(received, "exit") == 0) {
                send(clientSocket, received, recvSize, 0);
                std::cout << "> Disconnected." << std::endl;
                break;
            }
            send(clientSocket, received, recvSize, 0);
        }
    }

    clientThreads[threadN] = NULL;
    threadCnt--;

    closesocket(clientSocket);
    _endthreadex(0);
    return 0;
}
