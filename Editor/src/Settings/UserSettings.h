#pragma once
#include <Andromeda.h>
#include <filesystem>

namespace andromeda {
	class UserSettings {
    public:
        Vector2 windowSize{};
        Vector2 windowPosition{};
        bool maximized = true;

        void Save(const std::filesystem::path& path);
        bool Load(const std::filesystem::path& path);
    private:
        std::filesystem::path m_settingsPath;
    };
} // namespace andromeda