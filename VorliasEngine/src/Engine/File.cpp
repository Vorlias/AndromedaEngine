#include "Engine/File.h"
#include "Engine/Platform.h"
#include <fstream>
#include <sstream>
#include <filesystem>
#include <cstdlib>

namespace andromeda {
	std::string ReadFile(const std::string& filePath) {
		std::ifstream infile(filePath);
		if (infile.is_open()) {
			std::stringstream buffer;
			buffer << infile.rdbuf();
			const std::string output = buffer.str();
			infile.close();
			return output;
		}
		return std::string();
	}

	const std::filesystem::path GetDirectory(DirectoryType dirType) {
#if ANDROMEDA_LINUX
		switch (dirType) {
			case DirectoryType::PersistentData: {
				auto homeDir = std::getenv("HOME");
				if (homeDir == nullptr)
					return std::filesystem::current_path() / "config";

				return std::filesystem::path(homeDir) / ".config" / "andromeda";
			}
			case DirectoryType::ApplicationData: {
				return std::filesystem::current_path() / "data";
			}
		}
#else
#	error GetDirectory not implemented for current platform
#endif
	}
} // namespace andromeda