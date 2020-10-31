#include "ModuleNetworkingServer.h"

/*
	Oriol Capdevila & Christian Martínez
*/

//////////////////////////////////////////////////////////////////////
// ModuleNetworkingServer public methods
//////////////////////////////////////////////////////////////////////

bool ModuleNetworkingServer::start(int port)
{
	// TODO(jesus): TCP listen socket stuff
	// - Create the listenSocket
	if ((listenSocket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
		reportError("Invalid server listen socket");
		return false;
	}

	// - Set address reuse
	sockaddr_in remoteAddr;
	remoteAddr.sin_family = AF_INET;
	remoteAddr.sin_port = htons(port);
	remoteAddr.sin_addr.S_un.S_addr = INADDR_ANY;

	// - Bind the socket to a local interface
	if (bind(listenSocket, (const struct sockaddr*)&remoteAddr, sizeof(remoteAddr)) == SOCKET_ERROR) {
		reportError("Cannot bind listen socket");
		return false;
	}

	// - Enter in listen mode
	if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {
		reportError("Error listen listenSocket");
		return false;
	}

	// - Add the listenSocket to the managed list of sockets using addSocket()
	addSocket(listenSocket);

	state = ServerState::Listening;

	return true;
}

bool ModuleNetworkingServer::isRunning() const
{
	return state != ServerState::Stopped;
}



//////////////////////////////////////////////////////////////////////
// Module virtual methods
//////////////////////////////////////////////////////////////////////

bool ModuleNetworkingServer::update()
{
	return true;
}

bool ModuleNetworkingServer::gui()
{
	if (state != ServerState::Stopped)
	{
		// NOTE(jesus): You can put ImGui code here for debugging purposes
		ImGui::Begin("Server Window");

		Texture *tex = App->modResources->server;
		ImVec2 texSize(400.0f, 400.0f * tex->height / tex->width);
		ImGui::Image(tex->shaderResource, texSize);

		ImGui::Text("List of connected sockets:");

		for (auto &connectedSocket : connectedSockets)
		{
			ImGui::Separator();
			ImGui::Text("Socket ID: %d", connectedSocket.socket);
			ImGui::Text("Address: %d.%d.%d.%d:%d",
				connectedSocket.address.sin_addr.S_un.S_un_b.s_b1,
				connectedSocket.address.sin_addr.S_un.S_un_b.s_b2,
				connectedSocket.address.sin_addr.S_un.S_un_b.s_b3,
				connectedSocket.address.sin_addr.S_un.S_un_b.s_b4,
				ntohs(connectedSocket.address.sin_port));
			ImGui::Text("Player name: %s", connectedSocket.playerName.c_str());
		}

		ImGui::End();
	}

	return true;
}



//////////////////////////////////////////////////////////////////////
// ModuleNetworking virtual methods
//////////////////////////////////////////////////////////////////////

bool ModuleNetworkingServer::isListenSocket(SOCKET socket) const
{
	return socket == listenSocket;
}

void ModuleNetworkingServer::onSocketConnected(SOCKET socket, const sockaddr_in &socketAddress)
{
	// Add a new connected socket to the list
	ConnectedSocket connectedSocket;
	connectedSocket.socket = socket;
	connectedSocket.address = socketAddress;
	connectedSockets.push_back(connectedSocket);
}

void ModuleNetworkingServer::onSocketReceivedData(SOCKET socket, const InputMemoryStream& packet)
{
	ClientMessage clientMessage;
	packet >> clientMessage;

	switch (clientMessage)
	{
	case ClientMessage::Hello: {
		std::string playerName;
		packet >> playerName;

		for (auto connectedSocket = connectedSockets.begin(); connectedSocket != connectedSockets.end(); ++connectedSocket) {
			if ((*connectedSocket).playerName.compare(playerName) == 0) {
				OutputMemoryStream p;
				p << ServerMessage::NameAlreadyUsed;
				p << "Name already used";
				sendPacket(p, socket);
				connectedSockets.erase(connectedSocket);
				break;
			}
		}

		for (auto& connectedSocket : connectedSockets) {
			if (connectedSocket.socket == socket)
				connectedSocket.playerName = playerName;

			OutputMemoryStream p;
			p << ServerMessage::Welcome;
			if (socket == connectedSocket.socket)
				p << "You";
			else
				p << playerName;

			sendPacket(p, connectedSocket.socket);
		}
	}
		break;
	case ClientMessage::Message: {
		std::string msg;
		packet >> msg;

		if (msg[0] == '/') {
			auto it = commands.end();
			for (auto i = commands.begin(); i != commands.end(); ++i) {
				if (msg.find((*i).second) != std::string::npos) {
					it = i;
					break;
				}
			}

			if (it != commands.end()) {
				OutputMemoryStream p;
				switch ((*it).first)
				{
				case Commands::HELP: {
					p << ServerMessage::ServerText;
					std::string str = "Commands: ";
					for (auto i = commands.begin(); i != commands.end(); ++i) {
						if (i == commands.begin())
							str.append((*i).second);
						else
							str.append(", " + (*i).second);
					}

					p << str;
					sendPacket(p, socket);
					break;
				}
				case Commands::LIST: {
					p << ServerMessage::ServerText;
					std::string str = "List of users:\n";
					for (auto i = connectedSockets.begin(); i != connectedSockets.end(); ++i) {
						if ((*i).socket != socket)
							str.append((*i).playerName);
						else
							str.append("You");
						if (i != connectedSockets.end() - 1)
							str.append("\n");
					}

					p << str;
					sendPacket(p, socket);
					break;
				}
				case Commands::KICK: {
					std::string player_to_kick = GetArgument(msg, 0);
					if (player_to_kick.empty()) {
						p << ServerMessage::InvalidCommand;
						p << "Argument 1 is empty, must be a connected player";
						sendPacket(p, socket);
					}

					auto player_it = connectedSockets.end();

					for (auto connectedSocket = connectedSockets.begin(); connectedSocket != connectedSockets.end(); ++connectedSocket) {
						if (player_to_kick.compare((*connectedSocket).playerName) == 0) {
							player_it = connectedSocket;
							break;
						}
					}

					if (player_it != connectedSockets.end()) {
						p << ServerMessage::ToKick;
						p << "byebye";
						sendPacket(p, (*player_it).socket);
						connectedSockets.erase(player_it);

						for (auto& connectedSocket : connectedSockets) {
							OutputMemoryStream packet;
							packet << ServerMessage::Kicked;
							packet << player_to_kick + " was kicked";
							sendPacket(packet, connectedSocket.socket);
						}
					}
					else {
						p << ServerMessage::InvalidCommand;
						p << "Player " + player_to_kick + " not found to kick";
						sendPacket(p, socket);
					}

					break;
				}
				default:
					break;
				}
			}
			else {
				OutputMemoryStream p;
				p << ServerMessage::InvalidCommand;
				p << "Invalid Command";
				sendPacket(p, socket);
			}
			break;
		}

		std::string player;
		for (auto& connectedSocket : connectedSockets) {
			if (connectedSocket.socket == socket) {
				player = connectedSocket.playerName;
				break;
			}
		}

		for (auto& connectedSocket : connectedSockets) {
			if (connectedSocket.socket == socket)
				continue;
			OutputMemoryStream p;
			p << ServerMessage::Text;
			p << player + ": " + msg;

			sendPacket(p, connectedSocket.socket);
		}
		break;
	}
	default:
		break;
	}
}

void ModuleNetworkingServer::onSocketDisconnected(SOCKET socket)
{
	// Remove the connected socket from the list
	std::string playerDisconnected;

	for (auto it = connectedSockets.begin(); it != connectedSockets.end(); ++it)
	{
		auto &connectedSocket = *it;
		if (connectedSocket.socket == socket)
		{
			playerDisconnected.assign(connectedSocket.playerName);
			connectedSockets.erase(it);
			break;
		}
	}

	for (auto& connectedSocket : connectedSockets) {
		OutputMemoryStream packet;
		packet << ServerMessage::Disconnect;
		packet << playerDisconnected;
		sendPacket(packet, connectedSocket.socket);
	}
}

std::string ModuleNetworkingServer::GetArgument(const std::string& str, int argPos)
{
	std::string ret;

	int argn = 0;
	bool record = false;

	for (auto i = str.begin(); i != str.end(); ++i) {
		if (*i == ' ') {
			if (record) {
				break;
			}
			if (argn == argPos) {
				record = true;
			}
			else {
				argn++;
			}
		}
		else if (record)
			ret += *i;
	}

	return ret;
}

