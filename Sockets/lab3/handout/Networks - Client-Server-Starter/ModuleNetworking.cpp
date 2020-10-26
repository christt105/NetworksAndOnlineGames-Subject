#include "Networks.h"
#include "ModuleNetworking.h"

#include <list>

/*
	Oriol Capdevila & Christian Martínez
*/


static uint8 NumModulesUsingWinsock = 0;



void ModuleNetworking::reportError(const char* inOperationDesc)
{
	LPVOID lpMsgBuf;
	DWORD errorNum = WSAGetLastError();

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		errorNum,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf,
		0, NULL);

	ELOG("Error %s: %d- %s", inOperationDesc, errorNum, lpMsgBuf);
}

void ModuleNetworking::disconnect()
{
	for (SOCKET socket : sockets)
	{
		shutdown(socket, 2);
		closesocket(socket);
	}

	sockets.clear();
}

bool ModuleNetworking::init()
{
	if (NumModulesUsingWinsock == 0)
	{
		NumModulesUsingWinsock++;

		WORD version = MAKEWORD(2, 2);
		WSADATA data;
		if (WSAStartup(version, &data) != 0)
		{
			reportError("ModuleNetworking::init() - WSAStartup");
			return false;
		}
	}

	return true;
}

bool ModuleNetworking::preUpdate()
{
	if (sockets.empty()) return true;

	// NOTE(jesus): You can use this temporary buffer to store data from recv()
	const uint32 incomingDataBufferSize = Kilobytes(1);
	byte incomingDataBuffer[incomingDataBufferSize];

	// TODO(jesus): select those sockets that have a read operation available
	// New socket set
	fd_set readfds;
	FD_ZERO(&readfds);
	// Fill the set
	for (auto s : sockets) {
		FD_SET(s, &readfds);
	}
	// Timeout (return immediately)
	struct timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 0;
	// Select (check for readability)
	int res = select(0, &readfds, nullptr, nullptr, &timeout);
	if (res == SOCKET_ERROR) {
		reportError("select 4 read");
		return false;
	}
	// Fill this array with disconnected sockets
	std::list<SOCKET> disconnectedSockets;
	// TODO(jesus): for those sockets selected, check wheter or not they are
	// a listen socket or a standard socket and perform the corresponding
	// operation (accept() an incoming connection or recv() incoming data,
	// respectively).
	// Read selected sockets
	for (auto s : sockets)
	{
		if (FD_ISSET(s, &readfds)) {
			if (isListenSocket(s)) { // Is the server socket
			// Accept stuff
			// On accept() success, communicate the new connected socket to the
			// subclass (use the callback onSocketConnected()), and add the new
			// connected socket to the managed list of sockets.
				sockaddr_in sockAddr;
				int len = sizeof(sockAddr);
				SOCKET sock = accept(s, (struct sockaddr*)&sockAddr, &len);
				if (sock == INVALID_SOCKET) {
					reportError("Error accepting socket");
					continue;
				}
				onSocketConnected(sock, sockAddr);
				addSocket(sock);
			}
			else { // Is a client socket
			// Recv stuff
			// On recv() success, communicate the incoming data received to the
			// subclass (use the callback onSocketReceivedData()).
				//memset(incomingDataBuffer, '\0', incomingDataBufferSize);
				int ret = recv(s, (char*)incomingDataBuffer, incomingDataBufferSize, 0);
				// TODO(jesus): handle disconnections. Remember that a socket has been
				// disconnected from its remote end either when recv() returned 0,
				// or when it generated some errors such as ECONNRESET.
				// Communicate detected disconnections to the subclass using the callback
				// onSocketDisconnected().
				if (ret == SOCKET_ERROR || ret == ECONNRESET || ret == 0) {
					disconnectedSockets.push_back(s);
				}
				else {
					onSocketReceivedData(s, incomingDataBuffer);
				}
			}
		}
	}
	// Remove all disconnectedSockets from our list (sockets)
	// TODO(jesus): Finally, remove all disconnected sockets from the list
	// of managed sockets.
	for (auto s : disconnectedSockets) {
		onSocketDisconnected(s);
		sockets.erase(std::find(sockets.begin(), sockets.end(), s));
		if (closesocket(s) == SOCKET_ERROR) {
			reportError("close socket error ");
		}
	}

	return true;
}

bool ModuleNetworking::cleanUp()
{
	disconnect();

	NumModulesUsingWinsock--;
	if (NumModulesUsingWinsock == 0)
	{

		if (WSACleanup() != 0)
		{
			reportError("ModuleNetworking::cleanUp() - WSACleanup");
			return false;
		}
	}

	return true;
}

void ModuleNetworking::addSocket(SOCKET socket)
{
	sockets.push_back(socket);
}
