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
		Unknown,
		LuauScript,
	};

	inline const std::string GetAssetTypeName(AssetType assetType) {
		switch (assetType) {
			case AssetType::LuauScript:
				return "LuauScript";
		}

		return "";
	}

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
