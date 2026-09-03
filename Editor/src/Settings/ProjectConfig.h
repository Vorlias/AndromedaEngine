#pragma once
#include <Andromeda.h>
#include <filesystem>
#include <set>

namespace andromeda {

	class ProjectConfig {
	public:
		void Save(const std::filesystem::path& path);
		bool Load(const std::filesystem::path& path);

		std::string ProjectName = "AndromedaGame";
        std::string CompanyName = "Andromeda";
		std::string ActiveScenePath = "";

		struct BuildConfig {
			std::set<Platform> Platforms{Platform::Linux, Platform::Windows};
			std::string StartScenePath = "";
		} BuildConfig;

	private:
	};
} // namespace andromeda