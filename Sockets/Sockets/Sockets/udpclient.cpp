#include <iostream>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <chrono>
#include <string>

void printWSError(const char* msg)
{
	wchar_t* s = NULL;
	FormatMessageW(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM
		| FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPWSTR)&s,
		0, NULL);
	fprintf(stderr, "%s: %S\n", msg, s);
	LocalFree(s);
}

int main(int argc, char** argv) {
	std::cout << "UDP Client" << std::endl;
	for (int i = 1; i < argc; ++i)
		std::cout << "argc " << i << " argv " << argv[i] << std::endl;

	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR) {
		printWSError("CANNOT INITIALIZE CLIENT");
		return -1;
	}

	SOCKET s = socket(AF_INET, SOCK_DGRAM, 0);
	if (s == INVALID_SOCKET) {
		printWSError("CANNOT CREATE THE SOCKET");
	}

	sockaddr_in binAddr;
	binAddr.sin_family = AF_INET;
	binAddr.sin_port = htons(8000);
	inet_pton(AF_INET, "127.0.0.1", &binAddr.sin_addr);

	std::cout << "SOCKET CLIENT CREATED" << std::endl;

	for (int i = 0; i < 5; ++i) {
		std::cout << "Press any key to calculate ping to server" << std::endl;
		system("pause");
		std::string time = std::to_string(std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now()).time_since_epoch().count());
		if (sendto(s, time.c_str(), time.length(), 0, (const sockaddr*)&binAddr, sizeof(binAddr)) == SOCKET_ERROR) {
			printWSError("CANNOT SENDTO");
		}
	}

	std::cout << "Cleaning up the socket..." << std::endl;

	if (closesocket(s) == -1) {
		printWSError("CANNOT INITIALIZE CLIENT");
	}

	iResult = WSACleanup();
	if (iResult != NO_ERROR) {
		printWSError("CANNOT CLEAN UP CLIENT");
	}

	return iResult;
}
