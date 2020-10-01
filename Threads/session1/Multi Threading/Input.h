#pragma once

enum InputButtonState { Idle, Press, Pressed, Release };

struct InputController
{
	bool isConnected = false;

	float leftThumbX = 0.0f;
	float leftThumbY = 0.0f;

	union
	{
		InputButtonState buttons[12];
		struct
		{
			InputButtonState moveUp;
			InputButtonState moveDown;
			InputButtonState moveLeft;
			InputButtonState moveRight;
			InputButtonState actionUp;
			InputButtonState actionDown;
			InputButtonState actionLeft;
			InputButtonState actionRight;
			InputButtonState leftShoulder;
			InputButtonState rightShoulder;
			InputButtonState start;
			InputButtonState select;
		};
	};
};

// NOTE(jesus): Global object to access the input controller
extern InputController Input;
