#pragma once

// NOTE(jesus):
// Use log just like standard printf function.
// Example: LOG("New user connected %s\n", usernameString);

#define LOG(format, ...) log(__FILE__, __LINE__, format, __VA_ARGS__)

void log(const char file[], int line, const char* format, ...);

int logLineCount();

const char* logLineAt(int index);
