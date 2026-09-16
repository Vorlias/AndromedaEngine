#pragma once
#include "../Asset.h"
#include "../Data/Image.h"
#include "../Data/Vector.h"
#include "../Data/Rect.h"

namespace andromeda {
	class Texture2DAsset : public Asset {
	public:
		Texture2DAsset(UUID uuid, const std::filesystem::path& path, Image::UniqueRef image);

		const Image& GetImage() const {
			return *m_image;
		}

		void ReplaceImage(Image::UniqueRef newImage) {
			m_image = std::move(newImage);
		}
	private:
		Image::UniqueRef m_image;
	};

	class SpriteAsset : public Asset {
	public:
		SpriteAsset(UUID uuid, const Texture2DAsset& texture, IntRect spriteRect)
			: m_texture(texture)
			, m_spriteRect(spriteRect)
			, Asset(AssetType::Sprite, uuid, texture.GetFilePath()) {};

		const Image& GetImage() const {
			return m_texture.GetImage();
		}

	private:
		const Texture2DAsset& m_texture;
		const IntRect m_spriteRect;
	};
} // namespace andromeda


inline andromeda::Texture2DAsset::Texture2DAsset(UUID uuid, const std::filesystem::path& path, Image::UniqueRef image)
	: Asset(AssetType::Texture2D, uuid, path), m_image(std::move(image)) {}