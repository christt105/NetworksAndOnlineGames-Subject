#include <iostream>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

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

int CleanUp()
{
	int iResult = WSACleanup();
	if (iResult != NO_ERROR) {
		printWSError("CANNOT CLEAN UP CLIENT");
		return -1;
	}

	return 0;
}

int main(int argc, char** argv) {
	std::cout << "UDP Client" << std::endl;
	for (int i = 0; i < argc; ++i)
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
		remoteAddr.sin_port = htons(8888);
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
			if (recvfrom(s, buf, 32, 0, (struct sockaddr*)&sockAddr, &len) != SOCKET_ERROR) {
				printf("Received packet from %s:%d\n", inet_ntoa(sockAddr.sin_addr), ntohs(sockAddr.sin_port));
				printf("Data: %s\n", buf);
			}

			/*if ((recv_len = recvfrom(s, buf, 32, 0, (struct sockaddr*)&sockAddr, &len)) == SOCKET_ERROR)
			{
				printf("recvfrom() failed with error code : %d", WSAGetLastError());
				exit(EXIT_FAILURE);
			}

			//print details of the client/peer and the data received
			printf("Received packet from %s:%d\n", inet_ntoa(sockAddr.sin_addr), ntohs(sockAddr.sin_port));
			printf("Data: %s\n", buf);*/
		}

		std::cout << "Cleaning up the socket..." << std::endl;

		if (closesocket(s) == -1) {
			printWSError("CANNOT INITIALIZE SERVER");
		}
	}

	int retval = CleanUp();

	system("pause");
	return retval;
}