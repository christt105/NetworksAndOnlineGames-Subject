#include <iostream>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <chrono>
#include <string>

#include "globals.h"

int UDPServer(int argc, char** argv) {
	std::cout << "UDP Server" << std::endl;
	for (int i = 1; i < argc; ++i)
		std::cout << "argc " << i << " argv " << argv[i] << std::endl;

	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR) {
		printWSError("CANNOT INITIALIZE SERVER");
		return -1;
	}

	SOCKET s = socket(AF_INET, SOCK_DGRAM, 0);
	if (s != INVALID_SOCKET) {

		sockaddr_in remoteAddr;
		remoteAddr.sin_family = AF_INET;
		remoteAddr.sin_port = htons(8000);
		remoteAddr.sin_addr.S_un.S_addr = INADDR_ANY;

		std::cout << "SOCKET SERVER CREATED" << std::endl;

		int res = bind(s, (const struct sockaddr*)&remoteAddr, sizeof(remoteAddr));
		if (res == SOCKET_ERROR) {
			printWSError("CANNOT BIND");
		}

		bool loop = true;
		char buf[32];
		sockaddr_in sockAddr;
		int len, recv_len;

		len = sizeof(sockAddr);

		while (loop) {
			memset(buf, '\0', 32);
			if (recvfrom(s, buf, 32, 0, (struct sockaddr*)&sockAddr, &len) == SOCKET_ERROR) {
				std::cout << "Error on recvfrom()" << std::endl;
				break;
			}
			printf("Received packet from %s:%d\n", inet_ntoa(sockAddr.sin_addr), ntohs(sockAddr.sin_port));
			long long now = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now()).time_since_epoch().count();
			long long start = std::stoll(buf);
			std::cout << "ping: " << (now - start) * 0.001f << "ms" << std::endl;
		}

		std::cout << "Cleaning up the socket..." << std::endl;

		if (closesocket(s) == -1) {
			printWSError("CANNOT INITIALIZE SERVER");
		}
	}

	iResult = WSACleanup();
	if (iResult != NO_ERROR) {
		printWSError("CANNOT CLEAN UP CLIENT");
	}

	return iResult;
}