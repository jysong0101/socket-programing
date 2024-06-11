#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include <winsock2.h>
#include <string>
#pragma comment(lib,"ws2_32.lib")

#define PORT 65000
#define PACKET_SIZE 1024

int main() {
	SOCKET serverSocket, clientSocket;
	SOCKADDR_IN serverAddress, clientAddress;

	char received[PACKET_SIZE];

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
	
	std::cout << "> Server On.\n" << "> Listening..." << std::endl;
	
	listen(serverSocket, SOMAXCONN);

	int clientSize = sizeof(clientAddress);

	clientSocket = accept(serverSocket, (SOCKADDR*)&clientAddress, &clientSize);

	if (!WSAGetLastError()) {
		std::cout << "> Connect complete." << std::endl;
	}

	while (1) {
		int length = recv(clientSocket, received, sizeof(received), 0);
		received[length] = NULL;
		std::cout << "> Received: " << received << std::endl;
		std::cout << "> Echoing to client: " << received << std::endl;
		if (strcmp(received, "exit") == 0) {
			send(clientSocket, received, sizeof(received) - 1, 0);
			std::cout << "> Disconnected." << std::endl;
			break;
		}
		send(clientSocket, received, sizeof(received) - 1, 0);
	}

	closesocket(clientSocket);
	closesocket(clientSocket);

	WSACleanup();
	return 0;
}