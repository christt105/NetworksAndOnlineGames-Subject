#include "ModuleNetworkingClient.h"


//////////////////////////////////////////////////////////////////////
// ModuleNetworkingClient public methods
//////////////////////////////////////////////////////////////////////


void ModuleNetworkingClient::setServerAddress(const char * pServerAddress, uint16 pServerPort)
{
	serverAddressStr = pServerAddress;
	serverPort = pServerPort;
}

void ModuleNetworkingClient::setPlayerInfo(const char * pPlayerName, uint8 pSpaceshipType)
{
	playerName = pPlayerName;
	spaceshipType = pSpaceshipType;
}

uint32 ModuleNetworkingClient::GetNetworkID() const
{
	return networkId;
}



//////////////////////////////////////////////////////////////////////
// ModuleNetworking virtual methods
//////////////////////////////////////////////////////////////////////

void ModuleNetworkingClient::onStart()
{
	if (!createSocket()) return;

	if (!bindSocketToPort(0)) {
		disconnect();
		return;
	}

	// Create remote address
	serverAddress = {};
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(serverPort);
	int res = inet_pton(AF_INET, serverAddressStr.c_str(), &serverAddress.sin_addr);
	if (res == SOCKET_ERROR) {
		reportError("ModuleNetworkingClient::startClient() - inet_pton");
		disconnect();
		return;
	}

	state = ClientState::Connecting;

	inputDataFront = 0;
	inputDataBack = 0;

	secondsSinceLastHello = 9999.0f;
	secondsSinceLastInputDelivery = 0.0f;
}

void ModuleNetworkingClient::onGui()
{
	if (state == ClientState::Stopped) return;

	if (ImGui::CollapsingHeader("ModuleNetworkingClient", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (state == ClientState::Connecting)
		{
			ImGui::Text("Connecting to server...");
		}
		else if (state == ClientState::Connected)
		{
			ImGui::Text("Connected to server");

			ImGui::Separator();

			ImGui::Text("Player info:");
			ImGui::Text(" - Id: %u", playerId);
			ImGui::Text(" - Name: %s", playerName.c_str());

			ImGui::Separator();

			ImGui::Text("Spaceship info:");
			ImGui::Text(" - Type: %u", spaceshipType);
			ImGui::Text(" - Network id: %u", networkId);

			vec2 playerPosition = {};
			GameObject *playerGameObject = App->modLinkingContext->getNetworkGameObject(networkId);
			if (playerGameObject != nullptr) {
				playerPosition = playerGameObject->position;
			}
			ImGui::Text(" - Coordinates: (%f, %f)", playerPosition.x, playerPosition.y);

			ImGui::Separator();

			ImGui::Text("Connection checking info:");
			ImGui::Text(" - Ping interval (s): %f", PING_INTERVAL_SECONDS);
			ImGui::Text(" - Disconnection timeout (s): %f", DISCONNECT_TIMEOUT_SECONDS);

			ImGui::Separator();

			ImGui::Text("Input:");
			ImGui::InputFloat("Delivery interval (s)", &inputDeliveryIntervalSeconds, 0.01f, 0.1f, 4);
		}
	}
}

void ModuleNetworkingClient::onPacketReceived(const InputMemoryStream &packet, const sockaddr_in &fromAddress)
{
	// TODO(you): UDP virtual connection lab session

	uint32 protoId;
	packet >> protoId;
	if (protoId != PROTOCOL_ID) return;

	secondsSinceReceivedPacket = 0.f;

	if (state == ClientState::Connecting)
	{
		ServerMessage message;
		packet >> message;

		if (message == ServerMessage::Welcome)
		{
			if (delivery_manager.processSequenceNumber(packet)) {
				packet >> playerId;
				packet >> networkId;

				LOG("ModuleNetworkingClient::onPacketReceived() - Welcome from server");
				state = ClientState::Connected;
			}
		}
		else if (message == ServerMessage::Unwelcome)
		{
			WLOG("ModuleNetworkingClient::onPacketReceived() - Unwelcome from server :-(");
			disconnect();
		}
	}
	else if (state == ClientState::Connected)
	{
		ServerMessage message;
		packet >> message;

		switch (message) {
		case ServerMessage::Replication: {
			// TODO(you): World state replication lab session
			if (delivery_manager.processSequenceNumber(packet)) {
				replication_client.read(packet);
			}	
			break; }
		case ServerMessage::Ping: {
			//TODO:? why empty
			break; }
		case ServerMessage::Reliability: {
		// TODO(you): Reliability on top of UDP lab session
			uint32 i = 0U;
			packet >> i;
			inputDataFront = i;
			break; }
		case ServerMessage::PendingAck: {
			if (delivery_manager.processSequenceNumber(packet))
				delivery_manager.processAckdSequenceNumbers(packet);
			break; }
		case ServerMessage::ChangeNetworkID: {
			if (delivery_manager.processSequenceNumber(packet))
				packet >> networkId;
			break; }
		}
	}
}

void ModuleNetworkingClient::onUpdate()
{
	if (state == ClientState::Stopped) return;


	// TODO(you): UDP virtual connection lab session


	if (state == ClientState::Connecting)
	{
		secondsSinceLastHello += Time.deltaTime;

		if (secondsSinceLastHello > 0.1f)
		{
			secondsSinceLastHello = 0.0f;

			OutputMemoryStream packet;
			packet << PROTOCOL_ID;
			packet << ClientMessage::Hello;
			packet << playerName;
			packet << spaceshipType;

			delivery_manager.clear();

			sendPacket(packet, serverAddress);
		}
	}
	else if (state == ClientState::Connected)
	{
		// TODO(you): UDP virtual connection lab session
		secondsSinceReceivedPacket += Time.deltaTime;
		secondsSinceLastPing += Time.deltaTime;

		if (secondsSinceReceivedPacket > DISCONNECT_TIMEOUT_SECONDS) {
			disconnect();
		}

		if (secondsSinceLastPing > PING_INTERVAL_SECONDS) {
			OutputMemoryStream packet;
			packet << PROTOCOL_ID;
			packet << ClientMessage::Ping;
			sendPacket(packet, serverAddress);
			secondsSinceLastPing = 0.f;
		}

		// Process more inputs if there's space
		if (inputDataBack - inputDataFront < ArrayCount(inputData))
		{
			// Pack current input
			uint32 currentInputData = inputDataBack++;
			InputPacketData& inputPacketData = inputData[currentInputData % ArrayCount(inputData)];
			inputPacketData.sequenceNumber = currentInputData;
			inputPacketData.horizontalAxis = Input.horizontalAxis;
			inputPacketData.verticalAxis = Input.verticalAxis;
			inputPacketData.buttonBits = packInputControllerButtons(Input);
		}

		secondsSinceLastInputDelivery += Time.deltaTime;

		// Input delivery interval timed out: create a new input packet
		if (secondsSinceLastInputDelivery > inputDeliveryIntervalSeconds)
		{
			secondsSinceLastInputDelivery = 0.0f;

			OutputMemoryStream packet;
			packet << PROTOCOL_ID;
			packet << ClientMessage::Input;

			// TODO(you): Reliability on top of UDP lab session

			for (uint32 i = inputDataFront; i < inputDataBack; ++i)
			{
				InputPacketData& inputPacketData = inputData[i % ArrayCount(inputData)];
				packet << inputPacketData.sequenceNumber;
				packet << inputPacketData.horizontalAxis;
				packet << inputPacketData.verticalAxis;
				packet << inputPacketData.buttonBits;
			}

			// Clear the queue
			//inputDataFront = inputDataBack;

			sendPacket(packet, serverAddress);
		}

		// TODO(you): Latency management lab session

		// Update camera for player
		GameObject* playerGameObject = App->modLinkingContext->getNetworkGameObject(networkId);
		if (playerGameObject != nullptr)
		{
			App->modRender->cameraPosition = playerGameObject->position;
			respawning = false;
		}
		else
		{
			if (!respawning) {
				OutputMemoryStream packet;
				packet << PROTOCOL_ID;
				packet << ClientMessage::Respawn;
				delivery_manager.writeSequenceNumber(packet, new DeliveryDelegate(serverAddress, false))->CopyPacket(packet);
				sendPacket(packet, serverAddress);
				respawning = true;
			}
		}

		if (delivery_manager.hasSequenceNumbersPendingAck())
		{
			OutputMemoryStream packet;
			packet << PROTOCOL_ID;
			packet << ClientMessage::PendingAck;
			auto del = delivery_manager.writeSequenceNumber(packet, new DeliveryDelegate(serverAddress, false));
			delivery_manager.writeSequenceNumbersPendingAck(packet);
			del->CopyPacket(packet);
			sendPacket(packet, serverAddress);
		}
	}
}

void ModuleNetworkingClient::onConnectionReset(const sockaddr_in & fromAddress)
{
	disconnect();
}

void ModuleNetworkingClient::onDisconnect()
{
	state = ClientState::Stopped;

	GameObject *networkGameObjects[MAX_NETWORK_OBJECTS] = {};
	uint16 networkGameObjectsCount;
	App->modLinkingContext->getNetworkGameObjects(networkGameObjects, &networkGameObjectsCount);
	App->modLinkingContext->clear();

	for (uint32 i = 0; i < networkGameObjectsCount; ++i)
	{
		Destroy(networkGameObjects[i]);
	}

	App->modRender->cameraPosition = {};
}
