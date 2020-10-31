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

	const uint32 incomingDataBufferSize = Kilobytes(1);
	byte incomingDataBuffer[incomingDataBufferSize];

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
	for (auto s : sockets)
	{
		if (FD_ISSET(s, &readfds)) {
			if (isListenSocket(s)) { // Is the server socket
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
				InputMemoryStream packet;
				int bytes = recv(s, packet.GetBufferPtr(), packet.GetCapacity(), 0);

				if (bytes == SOCKET_ERROR || bytes == ECONNRESET || bytes == 0) {
					disconnectedSockets.push_back(s);
				}
				else {
					if (bytes > 0)
						packet.SetSize((uint32)bytes);
					onSocketReceivedData(s, packet);
				}
			}
		}
	}
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

bool ModuleNetworking::sendPacket(const OutputMemoryStream& packet, SOCKET socket)
{
	if (send(socket, packet.GetBufferPtr(), packet.GetSize(), 0) == SOCKET_ERROR) {
		reportError("Error sending a packet");
		return false;

	}
	return true;
}

void ModuleNetworking::addSocket(SOCKET socket)
{
	sockets.push_back(socket);
}
