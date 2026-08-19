#pragma once
#include <filesystem>
#include "Memory.h"
#include <unordered_map>

#include <iostream>
#include <format>
#include <optional>

#include "Log.h"

namespace andromeda {
	enum class AssetType {
		LuauScript,
	};

	class UUID {
	public:
		UUID();
		UUID(uint64_t uuid);
		UUID(const UUID&) = default;

		operator uint64_t() const {
			return uuid;
		}

	private:
		uint64_t uuid;
	};

	class Asset : public RefCounted {
		friend class AssetLibrary;
	protected:
		Asset(AssetType type, UUID uuid, const std::string& path) : m_uuid(uuid), m_path(path), m_assetType(type) {}

		inline void SetFilePath(std::string_view path) {
			m_path = path;
		}

		inline void SetUUID(UUID uuid) {
			m_uuid = uuid;
		}

	public:
		[[nodiscard]] constexpr AssetType GetAssetType() const {
			return m_assetType;
		}

		[[nodiscard]] constexpr std::string GetFilePath() const {
			return m_path;
		}

		[[nodiscard]] const UUID GetUUID() const {
			return m_uuid;
		}

	private:
		std::string m_path;
		AssetType m_assetType;
		UUID m_uuid;
	};

#if ANDROMEDA_EDITOR
	struct AssetImportContext {
		std::filesystem::path assetPath;
	};

	class AssetImporter {
	protected:
		AssetImporter(const std::string& ext, int version) : m_ext(ext), m_version(version) {}

	public:
		virtual void OnImportAsset(const AssetImportContext& context) = 0;

		constexpr std::string GetExtension() const {
			return m_ext;
		}
		constexpr int GetVersion() const {
			return m_version;
		}

	private:
		std::string m_ext;
		int m_version;
	};
#endif

	struct AssetHandle {
		UUID id;
		AssetType assetType;
		std::string name;
		std::string path;
		Ref<Asset> asset;
	};

	class AssetLibrary {
	public:
#if ANDROMEDA_EDITOR
		template<class T>
			requires(std::is_base_of<AssetImporter, T>::value && !std::is_same<AssetImporter, T>::value)
		void RegisterImporter() {
			T newImporter;
			andromeda::print("Added importer for {}", newImporter.GetExtension());
			m_importers.insert({newImporter.GetExtension(), newImporter});
		}

		void ImportAsset(const std::filesystem::path& path) {
			auto extension = path.extension();
			auto name = path.stem();
			
			std::filesystem::path meta = path;
			meta += ".meta";
		}
#endif
		template<class T>
		void RegisterAsset(UUID uuid, const std::string& name, const std::string& path, Ref<T> asset) {
			AssetHandle handle;
			handle.id = uuid;
			handle.name = name;
			handle.path = path;

			Ref<Asset> assetBase = asset.template As<Asset>();
			assetBase->SetUUID(uuid);
			assetBase->SetFilePath(path);

			handle.asset = assetBase;

			andromeda::trace("Register asset '{}' ({}) from '{}'", name, (uint32_t)uuid, path);
			m_assets.insert({ path, handle });
		}

		template<class T>
			requires std::is_base_of<Asset, T>::value
		[[nodiscard]] Ref<T> LoadAsset(const std::string& path) {
			if (m_assets.contains(path)) {
				return m_assets.at(path).asset.As<T>();
			}

			return nullptr;
		}

	private:
#if ANDROMEDA_EDITOR
		std::unordered_map<std::string, const AssetImporter&> m_importers;
#endif
		std::unordered_map<std::string, AssetHandle> m_assets;
	};
} // namespace andromeda

namespace std {
	template<typename T>
	struct hash;

	template<typename T, typename U>
	struct formatter;

	template<>
	struct hash<andromeda::UUID> {
		std::size_t operator()(const andromeda::UUID& uuid) const {
			return (uint64_t)uuid;
		}
	};

	template<class CharT>
	struct formatter<andromeda::UUID, CharT> : formatter<uint32_t, CharT> {
		template<class FormatContext>
		auto format(const andromeda::UUID& obj, FormatContext& ctx) const {
			return formatter<uint32_t>::format((uint32_t)obj, ctx);
		}
	};
} // namespace std
