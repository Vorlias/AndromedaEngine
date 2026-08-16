#pragma once
#include "Engine/Time.h"
#include <initializer_list>

namespace andromeda {
	enum class Platform {
		Windows,
		Linux,
		Mac,
		Unknown,
	};

	constexpr Platform supportedPlatforms[] = {
		Platform::Linux,
	};

	inline static Platform GetPlatform() {
#if ANDROMEDA_WIN
		return Platform::Windows;
#elif ANDROMEDA_LINUX
		return Platform::Linux;
#elif ANDROMEDA_MAC
		return Platform::Mac;
#else
		return Platform::Unknown;
#endif
	}

	namespace path {
		const std::string join(std::initializer_list<std::string> list);
		const std::vector<std::string> components(const std::string& filePath);
	}

	static bool FileExists(const char* path);
} // namespace andromeda