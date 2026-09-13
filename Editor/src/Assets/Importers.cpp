#pragma once
#include "Importers.h"

#include "Assets.h"
#include "Engine/Data/Image.h"
#include "Engine/Assets/TextureAsset.h"

namespace andromeda {
	PNGImageImporter::PNGImageImporter() : AssetImporter("png", 1) {}
	void PNGImageImporter::OnImportAsset(const AssetImportContext& context) const {
		using namespace andromeda;

		auto image = Image::LoadFromFile(context.absoluteAssetPath);

		auto asset = context.assetLibrary->GetAsset(context.assetPath);
		if (asset) {
			auto texture = asset.As<Texture2DAsset>();
			texture->ReplaceImage(std::move(image));
		} else {
			auto newTexture = Ref<Texture2DAsset>::Create(UUID{}, context.absoluteAssetPath, std::move(image));
			context.assetLibrary->AddAsset(newTexture->GetAssetType(), context.assetPath.stem(), context.assetPath, newTexture);
		}
	}
} // namespace andromeda

namespace andromeda {
    GLTFModelImporter::GLTFModelImporter(): AssetImporter("gltf", 1) {};
	void GLTFModelImporter::OnImportAsset(const AssetImportContext& context) const {
    }
}
