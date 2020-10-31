#pragma once

#include "ModuleNetworking.h"

#include <unordered_map>

class ModuleNetworkingServer : public ModuleNetworking
{
public:

	//////////////////////////////////////////////////////////////////////
	// ModuleNetworkingServer public methods
	//////////////////////////////////////////////////////////////////////

	bool start(int port);

	bool isRunning() const;



private:

	enum class Commands {
		HELP,
		KICK,
		LIST,
		WHISPER,
		CHANGE_NAME
	};

	//////////////////////////////////////////////////////////////////////
	// Module virtual methods
	//////////////////////////////////////////////////////////////////////

	bool update() override;

	bool gui() override;



	//////////////////////////////////////////////////////////////////////
	// ModuleNetworking virtual methods
	//////////////////////////////////////////////////////////////////////

	bool isListenSocket(SOCKET socket) const override;

	void onSocketConnected(SOCKET socket, const sockaddr_in &socketAddress) override;

	void onSocketReceivedData(SOCKET socket, const InputMemoryStream& packet) override;

	void onSocketDisconnected(SOCKET socket) override;



	//////////////////////////////////////////////////////////////////////
	// State
	//////////////////////////////////////////////////////////////////////

	enum class ServerState
	{
		Stopped,
		Listening
	};

	ServerState state = ServerState::Stopped;

	SOCKET listenSocket = INVALID_SOCKET;

	struct ConnectedSocket
	{
		sockaddr_in address;
		SOCKET socket;
		std::string playerName;
	};

	std::vector<ConnectedSocket> connectedSockets;

	std::unordered_map<Commands, std::string> commands {
		{Commands::HELP, "help"},
		{Commands::LIST, "list"},
		{Commands::KICK, "kick"},
		{Commands::WHISPER, "whisper"},
		{Commands::CHANGE_NAME, "change_name"}
	};

	static std::string GetArgument(const std::string& str, int argPos);
};

