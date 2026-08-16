#pragma once
#include <filesystem>

namespace andromeda {
    std::string ReadFile(const std::string& filePath);

    enum class DirectoryType {
        PersistentData,
        ApplicationData,
    };
    const std::filesystem::path GetDirectory(DirectoryType dirType);
}