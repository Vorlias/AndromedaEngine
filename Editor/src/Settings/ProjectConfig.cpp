#include "ProjectConfig.h"
// #define TOML_IMPLEMENTATION
// #include "toml.hpp"

#include "yaml-cpp/yaml.h"

#include <fstream>

YAML::Emitter& operator<<(YAML::Emitter& out, const std::set<andromeda::Platform>& v) {
	out << YAML::BeginSeq;

	for (auto& value : v) {
		out << andromeda::PlatformToString(value);
	}

	out << YAML::EndSeq;

	return out;
}

namespace YAML {
	template<>
	struct convert<std::set<andromeda::Platform>> {
		static Node encode(const std::set<andromeda::Platform>& rhs) {
			Node node;
			return node;
		}

		static bool decode(const Node& node, std::set<andromeda::Platform>& rhs) {
			if (!node.IsSequence()) {
				return false;
			}
			rhs.clear();

			for (int i = 0; i < node.size(); i++) {
				rhs.insert(andromeda::StringToPlatform(node[i].as<std::string>()));
			}

			return true;
		}
	};
} // namespace YAML

bool andromeda::ProjectConfig::Load(const std::filesystem::path& path) {
	if (std::filesystem::exists(path)) {
		std::ifstream stream(path);
		std::stringstream sstream;

		sstream << stream.rdbuf();

		YAML::Node data = YAML::Load(sstream.str());

		auto name = data["project_name"].as<std::string>();
		auto active_scene = data["active_scene"].as<std::string>();

		ProjectName = name;
		ActiveScenePath = active_scene;
        CompanyName = data["company_name"].as<std::string>();

		auto platforms = data["build_config"]["platforms"];
		if (platforms.IsSequence()) {
			BuildConfig.Platforms = platforms.as<std::set<andromeda::Platform>>();
		}

		return true;
	}

	return false;
}

void andromeda::ProjectConfig::Save(const std::filesystem::path& path) {
	YAML::Emitter yaml;

	yaml << YAML::BeginMap;
	{
		yaml << YAML::Key << "project_name" << YAML::Value << ProjectName;
        yaml << YAML::Key << "company_name" << YAML::Value << CompanyName;

		yaml << YAML::Key << "active_scene" << YAML::Value << ActiveScenePath;

		yaml << YAML::Key << "build_config" << YAML::Value << YAML::BeginMap;
		{
			yaml << YAML::Key << "platforms" << YAML::Value << YAML::Flow << BuildConfig.Platforms;
			yaml << YAML::Key << "scene" << YAML::Value << BuildConfig.StartScenePath;
			yaml << YAML::Key << "scenes" << YAML::Value << YAML::Flow << YAML::BeginSeq;
			{}
			yaml << YAML::EndSeq;
		}
		yaml << YAML::EndMap;
	}
	yaml << YAML::EndMap;

	std::ofstream file(path);
	file << yaml.c_str();
	file.flush();
	file.close();
}