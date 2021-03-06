#pragma once

// Add as many messages as you need depending on the
// functionalities that you decide to implement.

enum class ClientMessage
{
	Hello,
	Message
};

enum class ServerMessage
{
	Intro,
	Welcome,
	Text,
	ServerText,
	InvalidCommand,
	NameAlreadyUsed,
	Kicked,
	Pokemon,
	ChangeName,
	Whisper,
	ToKick,
	Disconnect
};

