#include "server.h"

#include <iostream>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <chrono>
#include <string>

//#include "globals.h"

int TCPServer(int argc, char** argv) {
	std::cout << "TCP Server" << std::endl;
	for (int i = 1; i < argc; ++i)
		std::cout << "argc " << i << " argv " << argv[i] << std::endl;

	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR) {
		//printWSError("CANNOT INITIALIZE SERVER");
		std::cout << "Error INITIALIZE SERVER" << std::endl;
		return -1;
	}

	SOCKET s = socket(AF_INET, SOCK_STREAM, 0);
	if (s != INVALID_SOCKET) {

		sockaddr_in remoteAddr;
		remoteAddr.sin_family = AF_INET;
		remoteAddr.sin_port = htons(8000);
		remoteAddr.sin_addr.S_un.S_addr = INADDR_ANY;

		std::cout << "SOCKET SERVER CREATED" << std::endl;

		int res = bind(s, (const struct sockaddr*)&remoteAddr, sizeof(remoteAddr));
		if (res == SOCKET_ERROR) {
			//printWSError("CANNOT BIND");
			std::cout << "Error BIND" << std::endl;
			return -1;
		}

		res = listen(s, 1);

		if (res == SOCKET_ERROR) {
			//printWSError("ERROR listening");
			std::cout << "Error listening" << std::endl;
			return -1;
		}

		bool loop = true;
		char buf[32];
		sockaddr_in sockAddr;
		int len, recv_len;

		len = sizeof(sockAddr);

		SOCKET socket = accept(s, (struct sockaddr*)&sockAddr, &len);

		if (socket == INVALID_SOCKET) {
			//printWSError("ERROR accepting");
			std::cout << "Error accepting" << std::endl;
			return -1;
		}
		system("pause");
		while (loop) {
			memset(buf, '\0', 32);
			if (recv(socket, buf, 32, 0) == SOCKET_ERROR) {
				std::cout << "Error on recv()" << std::endl;
				break;
			}
			printf("Received packet from %s:%d\n", inet_ntoa(sockAddr.sin_addr), ntohs(sockAddr.sin_port));
			std::cout << "ping: " << buf << std::endl;
		}

		std::cout << "Cleaning up the socket..." << std::endl;

		if (closesocket(s) == -1) {
			//printWSError("CANNOT INITIALIZE SERVER");
			std::cout << "Error INITIALIZE SERVER" << std::endl;
		}
	}

	iResult = WSACleanup();
	if (iResult != NO_ERROR) {
		//printWSError("CANNOT CLEAN UP CLIENT");
		std::cout << "Error CLEAN UP CLIENT" << std::endl;
	}

	return iResult;
}