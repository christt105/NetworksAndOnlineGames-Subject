#include "ModuleNetworkingClient.h"

/*
	Oriol Capdevila & Christian Martínez
*/


bool  ModuleNetworkingClient::start(const char * serverAddressStr, int serverPort, const char *pplayerName)
{
	playerName = pplayerName;

	// TODO(jesus): TCP connection stuff
	// - Create the socket
	if ((s = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
		reportError("Could not create client socket");
		return false;
	}

	// - Create the remote address object
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(serverPort);
	inet_pton(AF_INET, serverAddressStr, &serverAddress.sin_addr);

	// - Connect to the remote address
	if (connect(s, (const sockaddr*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
		reportError("Could not connect to server");
		return false;
	}

	// - Add the created socket to the managed list of sockets using addSocket()
	addSocket(s);

	// If everything was ok... change the state
	state = ClientState::Start;

	return true;
}

bool ModuleNetworkingClient::isRunning() const
{
	return state != ClientState::Stopped;
}

bool ModuleNetworkingClient::update()
{
	if (state == ClientState::Start)
	{
		OutputMemoryStream packet;
		packet << ClientMessage::Hello;
		packet << playerName;

		if (sendPacket(packet, s)) {
			state = ClientState::Logging;
		}
		else {
			disconnect();
			state = ClientState::Stopped;
		}
	}

	return true;
}

#define MAXBUFFER 50

bool ModuleNetworkingClient::gui()
{
	if (state != ClientState::Stopped)
	{
		// NOTE(jesus): You can put ImGui code here for debugging purposes
		ImGui::Begin("Client Window");

		Texture *tex = App->modResources->client;
		ImVec2 texSize(400.0f, 400.0f * tex->height / tex->width);
		ImGui::Image(tex->shaderResource, texSize);

		ImGui::Text("%s connected to the server...", playerName.c_str());

		if(ImGui::BeginChild("##chat", ImVec2(ImGui::GetWindowWidth() * 0.95f, ImGui::GetWindowHeight() * 0.65f), true)) {
			for (auto i = chat.begin(); i != chat.end(); i++) {
				switch ((*i).first) {
				
				case ServerMessage::Disconnect:
				case ServerMessage::Kicked:
				case ServerMessage::Welcome:
					ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.f), (*i).second.c_str());
					break;
				case ServerMessage::InvalidCommand:
					ImGui::TextColored(ImVec4(1.0f, 0.f, 0.f, 1.f), (*i).second.c_str());
					break;
				case ServerMessage::ServerText:
				case ServerMessage::Whisper:
					ImGui::TextColored(ImVec4(1.f, 0.f, 1.f, 1.f), (*i).second.c_str());
					break;
				default:
					ImGui::Text((*i).second.c_str());
					break;
				}
			}

			ImGui::EndChild();
		}

		static char buffer[MAXBUFFER] = { '\0' };
		if (ImGui::InputText("##line", buffer, MAXBUFFER, ImGuiInputTextFlags_EnterReturnsTrue)) {
			if (buffer[0] == '/' && (std::strcmp(buffer, "/clear") == 0 || std::strncmp(buffer, "/clear ", 7) == 0)) {
				chat.clear();
			}
			else {
				OutputMemoryStream packet;
				packet << ClientMessage::Message;
				packet << buffer;
				chat.push_back(std::pair<ServerMessage, std::string>(ServerMessage::Text, std::string("You: ") + buffer));
				sendPacket(packet, s);
			}
			memset(buffer, '\0', MAXBUFFER);
			ImGui::SetKeyboardFocusHere();
		}

		ImGui::End();
	}

	return true;
}

void ModuleNetworkingClient::onSocketReceivedData(SOCKET socket, const InputMemoryStream& packet)
{
	ServerMessage m;
	std::string p;
	packet >> m;
	packet >> p;
	switch (m)
	{
	case ServerMessage::Welcome:
		chat.push_back(std::pair<ServerMessage, std::string>(m, p + " joined the party"));
		break;
	case ServerMessage::Disconnect:
		chat.push_back(std::pair<ServerMessage, std::string>(m, p + " disconnected"));
		break;
	case ServerMessage::ToKick:
		onSocketDisconnected(socket);
		break;
	case ServerMessage::ChangeName:
		playerName = p;
		break;
	case ServerMessage::NameAlreadyUsed:
		ELOG("Name %s already used", playerName.c_str());
		onSocketDisconnected(socket);
		break;
	default:
		chat.push_back(std::pair<ServerMessage, std::string>(m, p));
		break;
	}
}

void ModuleNetworkingClient::onSocketDisconnected(SOCKET socket)
{
	state = ClientState::Stopped;
	chat.clear();
}

