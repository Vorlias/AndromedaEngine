#pragma once
#include <filesystem>

namespace andromeda {
	class AssetLibrary;

	struct AssetImportContext {
		std::filesystem::path assetPath;
		AssetLibrary* assetLibrary;
	};

	class AssetImporter {
	protected:
		AssetImporter(const std::string ext, int version) : m_ext(ext), m_version(version) {}

	public:
		virtual void OnImportAsset(const AssetImportContext& context) const = 0;

		const std::string GetExtension() const {
			return m_ext;
		}
		constexpr int GetVersion() const {
			return m_version;
		}

	private:
		std::string m_ext = "";
		int m_version = 0;
	};

	class DefaultFileImporter : public AssetImporter {
	public:
		DefaultFileImporter() : AssetImporter("", 1) {}
		void OnImportAsset(const AssetImportContext& context) const {
			warn("Default importer for {}", context.assetPath.c_str());
		}
	};


} // namespace andromeda