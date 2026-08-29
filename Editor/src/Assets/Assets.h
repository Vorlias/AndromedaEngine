#pragma once
#include <filesystem>
#include "yaml-cpp/yaml.h"
#include "Engine/Asset.h"

#include <fstream>
#include <sstream>
#include "FileWatcher.h"
#include "Importers.h"

#define YAML_MAP(out, _EXPR) \
	out << YAML::BeginMap; \
	_EXPR; \
	out << YAML::EndMap;

#define YAML_KV(key, value) YAML::Key << key << YAML::Value << value

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
				out << YAML::Key << "assetType" << YAML::Value << (int)m_assetType;
			}
			out << YAML::EndMap;

			std::filesystem::path outPath = m_path;
			outPath += ".meta";

			std::ofstream ostream(outPath);
			ostream << out.c_str();
			ostream.flush();
		}

		bool Load() {
			std::filesystem::path outPath = m_path;
			outPath += ".meta";


			if (std::filesystem::exists(outPath)) {
				std::ifstream stream(outPath);
				std::stringstream sstream;

				sstream << stream.rdbuf();

				YAML::Node data = YAML::Load(sstream.str());

				m_name = data["name"].as<std::string>();
				m_version = data["fileVersion"].as<int>();
				m_guid = data["uuid"].as<uint64_t>();

				return true;
			}

			return false;
		}

		void SetName(std::string_view name) {
			m_name = name;
		}

		void SetUUID(UUID uuid) {
			m_guid = uuid;
		}

		void SetAssetType(AssetType assetType) {
			m_assetType = assetType;
		}

		std::string GetName() const {
			return m_name;
		}

		UUID GetUUID() const {
			return m_guid;
		}

		AssetType GetAssetType() const {
			return m_assetType;
		}

	private:
		std::filesystem::path m_path;
		std::string m_name;
		AssetType m_assetType{0};
		int m_version;
		UUID m_guid;
	};

	struct AssetHandle {
		UUID id{};
		AssetType assetType{0};
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
			requires(std::is_base_of<AssetImporter, T>::value && !std::is_same<AssetImporter, T>::value)
		void RegisterImporter(const std::string& extension) {
			T* importer = new T();
			m_importers.insert({extension, importer});
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
			const auto uuid = GetAssetUUIDFromPath(path);
			if (!uuid)
				return nullptr;
			return GetAssetFromUUID(uuid);
		}

		[[nodiscard]] const UUID GetAssetUUIDFromPath(const std::string& path) {
			if (!m_pathToUUID.contains(path))
				return UUID::None;

			return m_pathToUUID.at(path);
		}

		[[nodiscard]] const Ref<Asset> GetAssetFromUUID(const UUID uuid) {
			if (!m_uassets.contains(uuid))
				return nullptr;
			return m_uassets.at(uuid).asset;
		}

		template<class T>
			requires std::is_base_of_v<Asset, T>
		void AddAsset(AssetType assetType, const std::string name, const std::string path, Ref<T> asset) {
			AssetHandle handle;
			handle.path = path;
			handle.name = name;
			handle.asset = asset.template As<Asset>();
			handle.assetType = assetType;

			AssetMeta meta(path);
			if (meta.Load()) {
				handle.name = meta.GetName();
				handle.id = meta.GetUUID();
				handle.assetType = meta.GetAssetType();
			} else {
				meta.SetName(handle.name);
				meta.SetAssetType(handle.assetType);
				meta.SetUUID(handle.id);
			}
			meta.Save();

			m_uassets.insert({handle.id, handle});
			m_pathToUUID.insert({path, handle.id});
		}

		bool Initialize(const std::filesystem::path& projectPath, const std::filesystem::path& path) {
			RegisterImporter<DefaultFileImporter>();

			m_watcher = FileWatcher{path, 2s};
			return m_watcher.Watch([this, projectPath](std::filesystem::path path, FileStatus status) {
				if (path.extension() == ".meta")
					return;

				auto stem = path.stem().string();
				if (stem.starts_with("."))
					return;

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

				for (auto& [importerExtension, importer] : m_importers) {
					if (importerExtension == ext) {
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

		std::unordered_map<UUID, AssetHandle> m_uassets;
		// std::unordered_map<std::string, AssetHandle> m_assets;
		std::unordered_map<std::string, UUID> m_pathToUUID;
	};
}; // namespace andromeda