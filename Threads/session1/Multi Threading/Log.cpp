
#include <windows.h>
#include <stdio.h>
#include "Log.h"

#include <vector>
#include <string>

static std::vector<std::string> lines;

void log(const char file[], int line, const char* format, ...)
{
	static char tmp_string[4096];
	static char tmp_string2[4096];
	static va_list  ap;

	const char *basefile = file;
	const size_t filelen = strlen(file);
	for (size_t i = 0; i < filelen; ++i) {
		if (file[i] == '\\' || file[i] == '/') {
			basefile = file + i + 1;
		}
	}

	// Construct the string from variable arguments
	va_start(ap, format);
	vsprintf_s(tmp_string, 4096, format, ap);
	va_end(ap);
	sprintf_s(tmp_string2, 4096, "%s(%d) : %s\n", basefile, line, tmp_string);
	
	// Windows debug output
	OutputDebugString(tmp_string2);

	// Standard output
	printf("%s", tmp_string2);

	// Store the log in memory
	lines.push_back(tmp_string2);
}

int logLineCount()
{
	return (int)lines.size();
}

const char * logLineAt(int index)
{
	return lines[index].c_str();
}
