#pragma once

#include <WinSock2.h>

class ModuleNetworking : public Module
{
private:

	//////////////////////////////////////////////////////////////////////
	// Module virtual methods
	//////////////////////////////////////////////////////////////////////

	bool init() override;

	bool preUpdate() override;

	bool cleanUp() override;



	//////////////////////////////////////////////////////////////////////
	// Socket event callbacks
	//////////////////////////////////////////////////////////////////////

	virtual bool isListenSocket(SOCKET socket) const { return false; }

	virtual void onSocketConnected(SOCKET socket, const sockaddr_in &socketAddress) { }

	virtual void onSocketReceivedData(SOCKET s, const InputMemoryStream& packet) = 0;

	virtual void onSocketDisconnected(SOCKET s) = 0;

protected:

	static bool sendPacket(const OutputMemoryStream& packet, SOCKET socket);

	std::vector<SOCKET> sockets;

	void addSocket(SOCKET socket);

	void disconnect();

	static void reportError(const char *message);
};

