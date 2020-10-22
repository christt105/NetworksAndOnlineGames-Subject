#include <iostream>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

//#include "../UDP Server/globals.h"

int TCPClient(int argc, char** argv) {
	std::cout << "UDP Client" << std::endl;
	for (int i = 1; i < argc; ++i)
		std::cout << "argc " << i << " argv " << argv[i] << std::endl;

	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR) {
		//printWSError("CANNOT INITIALIZE CLIENT");
		std::cout << "CANNOT INITIALIZE CLIENT" << std::endl;
		return -1;
	}

	SOCKET s = socket(AF_INET, SOCK_STREAM, 0);
	if (s == INVALID_SOCKET) {
		//printWSError("CANNOT CREATE THE SOCKET");
		std::cout << "CANNOT CREATE THE SOCKET" << std::endl;
		return -1;
	}

	sockaddr_in binAddr;
	binAddr.sin_family = AF_INET;
	binAddr.sin_port = htons(8000);
	inet_pton(AF_INET, "127.0.0.1", &binAddr.sin_addr);

	int res = connect(s, (const sockaddr*)&binAddr, sizeof(binAddr));

	if (res == SOCKET_ERROR) {
		//printWSError("CANNOT CONNECT");
		std::cout << "CANNOT CONNECT" << std::endl;
		return -1;
	}

	std::cout << "SOCKET CLIENT CREATED" << std::endl;

	for (int i = 0; i < 5; ++i) {
		std::cout << "Press any key to calculate ping to server" << std::endl;
		system("pause");
		std::string time = "time";
		if (send(s, time.c_str(), time.length(), 0) == SOCKET_ERROR) {
			//printWSError("CANNOT SENDTO");
			std::cout << "CANNOT SENDTO" << std::endl;
		}
	}

	std::cout << "Cleaning up the socket..." << std::endl;

	if (closesocket(s) == -1) {
		//printWSError("CANNOT INITIALIZE CLIENT");
		std::cout << "CANNOT INITIALIZE CLIENT" << std::endl;
	}

	iResult = WSACleanup();
	if (iResult != NO_ERROR) {
		//printWSError("CANNOT CLEAN UP CLIENT");
		std::cout << "CANNOT CLEAN UP CLIENT" << std::endl;
	}

	return iResult;
}
