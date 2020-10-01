#pragma once

struct TimeStruct
{
	float time = 0.0f;      // NOTE(jesus): Time in seconds since the application started
	float deltaTime = 0.0f; // NOTE(jesus): The completion time in seconds since the last frame
};

// NOTE(jesus): Global object to access the time
extern TimeStruct Time;
