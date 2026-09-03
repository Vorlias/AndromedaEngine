#pragma once
#include "Importers.h"
#include "Assets.h"
#include "Andromeda.h"

#include "Engine/Graphics/Shader.h"

namespace andromeda {
	class LuauScriptImporter : public AssetImporter {
	public:
		LuauScriptImporter() : AssetImporter("luau", 1) {}
		void OnImportAsset(const AssetImportContext& context) const {
			auto source = ReadFile(context.absoluteAssetPath);

			auto asset = context.assetLibrary->GetAsset(context.assetPath);
			if (asset) {
				auto script = asset.As<LuauScript>();
				script->Compile(source, context.assetPath);
			} else {
				auto script = LuauScript::CreateScript(source, context.assetPath);
				context.assetLibrary->AddAsset(AssetType::LuauScript, context.assetPath.stem(), context.assetPath, script);
			}
		}
	};

	class FontImporterTTF : public AssetImporter {
	public:
		FontImporterTTF() : AssetImporter("ttf", 1) {}
		void OnImportAsset(const AssetImportContext& context) const {}
	};

	class SPIRVShaderImporter : public AssetImporter {
	public:
		SPIRVShaderImporter() : AssetImporter("spv", 1) {}
		void OnImportAsset(const AssetImportContext& context) const {}
	};
} // namespace andromeda