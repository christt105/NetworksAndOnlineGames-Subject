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
		CHANGE_NAME,
		CLEAR,
		CATCH,
		POKEMON
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
		std::vector<std::string>pokemon;
	};

	std::vector<ConnectedSocket> connectedSockets;

	std::unordered_map<Commands, std::string> commands {
		{Commands::HELP, "help"},
		{Commands::LIST, "list"},
		{Commands::KICK, "kick"},
		{Commands::WHISPER, "whisper"},
		{Commands::CHANGE_NAME, "change_name"},
		{Commands::CLEAR, "clear"},
		{Commands::CATCH, "catch"},
		{Commands::POKEMON, "pokemon"}
	};

	static std::string GetArgument(const std::string& str, int argPos, bool andForward = false);

	//Pokemon bot
	int msg_count = 0;
	int msg_to_spawn_pokemon = 5;
	std::vector<std::string>::iterator actual_pokemon;
	std::vector<std::string> pokemon_database;
};

