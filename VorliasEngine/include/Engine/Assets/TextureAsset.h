#pragma once
#include "../Asset.h"
#include "../Data/Image.h"
#include "../Data/Vector.h"
#include "../Data/Rect.h"

namespace andromeda {
	class TextureAsset : public Asset {
	public:
		TextureAsset(UUID uuid, const std::filesystem::path& path);

		const Image GetImage() const {
			return m_image;
		}

	private:
		Image m_image;
	};

	class SpriteAsset : public Asset {
	public:
		SpriteAsset(UUID uuid, const TextureAsset& texture, IntRect spriteRect)
			: m_texture(texture)
			, m_spriteRect(spriteRect)
			, Asset(AssetType::Sprite, uuid, texture.GetFilePath())
			, m_sprite(m_texture.GetImage(), spriteRect) {};

		const Image GetImage() const {
			return m_texture.GetImage();
		}

	private:
		const TextureAsset& m_texture;
		const IntRect m_spriteRect;
		const Sprite m_sprite;
	};
} // namespace andromeda


andromeda::TextureAsset::TextureAsset(UUID uuid, const std::filesystem::path& path) : Asset(AssetType::Texture, uuid, path), m_image(Image(path)) {}