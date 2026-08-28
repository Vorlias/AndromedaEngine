#pragma once
#include "Importers.h"
#include "Assets.h"
#include "Andromeda.h"

namespace andromeda {
	class LuauScriptImporter : public AssetImporter {
	public:
		LuauScriptImporter() : AssetImporter("luau", 1) {}
		void OnImportAsset(const AssetImportContext& context) const {
			auto source = ReadFile(context.assetPath);

			auto asset = context.assetLibrary->GetAsset(context.assetPath);
			if (asset) {
				auto script = asset.As<LuauScript>();
				script->Compile(source, context.assetPath);
			} else {
				auto script = LuauScript::CreateScript(source, context.assetPath);
				context.assetLibrary->AddAsset(context.assetPath.stem(), context.assetPath, script);
			}
		}
	};
} // namespace andromeda