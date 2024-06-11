#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include <winsock2.h>
#include <string>
#pragma comment(lib,"ws2_32.lib")

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 65000
#define PACKET_SIZE 1024

int main() {
	SOCKET clientSocket;
	SOCKADDR_IN serverAddress;
	std::string sent;
	char received[PACKET_SIZE];

	WSADATA wsa;

	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		std::cout << "WSAStartup failed" << std::endl;
		return 0;
	}

	clientSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	memset(&serverAddress, 0, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(SERVER_PORT);
	serverAddress.sin_addr.s_addr = inet_addr(SERVER_IP);

	while (1) {
		if (!connect(clientSocket, (SOCKADDR*)&serverAddress, sizeof(serverAddress))) break;
		std::cout << "> Seaching server..." << std::endl;
	}

	std::cout << "> Connect complete." << std::endl;
	while (1) {
		getline(std::cin, sent);
		if (sent == "") continue;
		send(clientSocket, sent.c_str(), sent.length(), 0);
		int length = recv(clientSocket, received, sizeof(received), 0);
		received[length] = NULL;
		if (strcmp(received, "exit") == 0) {
			std::cout << "> Disconnected.";
			break;
		}
		std::cout << "> Echoing by server : " << received << std::endl;
	}
	closesocket(clientSocket);

	WSACleanup();
	return 0;
}