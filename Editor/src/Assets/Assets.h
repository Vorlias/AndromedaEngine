#pragma once
#include <filesystem>
#include "yaml-cpp/yaml.h"
#include "Engine/Asset.h"

#include <fstream>
#include <sstream>
#include "FileWatcher.h"
#include "Importers.h"



namespace andromeda {
	class AssetMeta {
	public:
		AssetMeta(std::filesystem::path path) : m_path(path), m_version(1), m_guid() {}

		void Save() {
			YAML::Emitter out;

			out << YAML::BeginMap;
			{
				out << YAML::Key << "name" << YAML::Value << m_name.c_str();
				out << YAML::Key << "fileVersion" << YAML::Value << m_version;
				out << YAML::Key << "uuid" << YAML::Value << m_guid;
			}
			out << YAML::EndMap;

			std::filesystem::path outPath = m_path;
			outPath += ".meta";

			std::cout << "outPath is " << outPath << ": " << out.c_str() << std::endl;
		}

		void Load() {
			std::ifstream stream(m_path);
			std::stringstream sstream;

			sstream << stream.rdbuf();

			YAML::Node data = YAML::Load(sstream.str());

			m_name = data["name"].as<std::string>();
			m_version = data["fileVersion"].as<int>();
			m_guid = data["uuid"].as<uint64_t>();
		}

	private:
		std::filesystem::path m_path;
		std::string m_name;
		int m_version;
		UUID m_guid;
	};

	struct AssetHandle {
		UUID id;
		AssetType assetType;
		std::string name;
		std::string path;
		Ref<Asset> asset;
	};

	class AssetLibrary {
	public:
		template<class T>
			requires(std::is_base_of<AssetImporter, T>::value && !std::is_same<AssetImporter, T>::value)
		void RegisterImporter() {
			T* importer = new T();
			m_importers.insert({importer->GetExtension(), importer});
		}

		template<class T>
			requires std::is_base_of<Asset, T>::value
		[[nodiscard]] Ref<T> GetAsset(const std::string& path) {
			auto test = GetAsset(path);
			if (test != nullptr) {
				return test.As<T>();
			}

			return nullptr;
		}

		[[nodiscard]] const Ref<Asset> GetAsset(const std::string& path) {
			if (m_assets.contains(path)) {
				return m_assets.at(path).asset;
			}

			return nullptr;
		}

		template<class T>
			requires std::is_base_of_v<Asset, T>
		void AddAsset(const std::string name, const std::string path, Ref<T> asset) {
			ANDROMEDA_ASSERT(!m_assets.contains(path));
			AssetHandle handle;
			handle.path = path;
			handle.name = name;
			handle.asset = asset.template As<Asset>();

			m_assets.insert({ path, handle });
		}

		bool CheckAsset(const std::string& path) {
			return m_assets.contains(path);
		}

		bool Initialize(const std::filesystem::path& projectPath, const std::filesystem::path& path) {
			RegisterImporter<DefaultFileImporter>();

			m_watcher = FileWatcher{path, 2s};
			return m_watcher.Watch([this, projectPath](std::filesystem::path path, FileStatus status) {
				ProcessFile(std::filesystem::relative(path, projectPath), status);
			});
		}

		void Shutdown() {
			m_watcher.Stop();

			for (auto [_, importer] : m_importers) {
				delete importer;
			}

			m_importers.clear();
		}

	private:
		void ProcessFile(const std::filesystem::path& path, FileStatus status) {
			if (std::filesystem::is_directory(path)) {
				trace("Directory '{}' {}", path.string(), (int)status);
			} else {
				auto ext = path.extension().string().substr(1);
				AssetImportContext importContext{path, this};

				for (auto [_, importer] : m_importers) {
					if (importer->GetExtension() == ext) {
						importer->OnImportAsset(importContext);
						return;
					}
				}

				auto& defaultImporter = m_importers.at("");
				defaultImporter->OnImportAsset(importContext);
			}
		}

	private:
		FileWatcher m_watcher;
		std::unordered_map<std::string, AssetImporter*> m_importers;
		std::unordered_map<std::string, AssetHandle> m_assets;
	};
}; // namespace andromeda