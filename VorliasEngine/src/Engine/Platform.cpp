#include "Engine/Platform.h"
#include "Engine/Time.h"
#include <string>
#include <sstream>

std::vector<std::string> split(const std::string& s, char delim) {
	std::vector<std::string> parts;
	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delim)) {
		parts.push_back(item);
	}
	return parts;
}

#ifdef ANDROMEDA_WIN
#	include <windows.h>
#	define PATH_SEP '\\'
#	define PATH_SEPSTR "\\"
#	define LINE_ENDING "\r\n"

bool andromeda::FileExists(const char* path) {
	DWORD attr = GetFileAttributesA(path);
	return attr != INVALID_FILE_ATTRIBUTES;
}
#else
#	include <sys/stat.h>
#	define PATH_SEP '/'
#	define PATH_SEPSTR "/"
#	define LINE_ENDING "\n"

bool andromeda::FileExists(const char* path) {
	struct stat st;
	return stat(path, &st) == 0;
}

// int32_t secondsToMilliseconds(float seconds) {
// 	return (int32_t)(seconds * 1'000);
// }

// long secondsToMicroseconds(float seconds) {
// 	return (int64_t)(seconds * 1'000'000);
// }

// void andromedasleep(time_t seconds) {
// 	timespec ti{
// 		.tv_sec = static_cast<time_t>(seconds),
// 		.tv_nsec = static_cast<long>(seconds * 1'000'000),
// 	};

// 	while ((nanosleep(&ti, &ti) == -1) && (errno == EINTR)) {}
// }
#endif

const std::vector<std::string> andromeda::PathComponents(const std::string& filePath) {
	return split(filePath, PATH_SEP);
}