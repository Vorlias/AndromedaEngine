#pragma once
#include "Vector.h"
#include <filesystem>
#include "SDL3/SDL.h"
#include "Rect.h"

namespace andromeda {
	struct Image {
		enum ImageType {
			ImageType_RawData,
			ImageType_STBI,
		} const type;

		enum Channels {
			// greyscale
			W = 1,
			// greyscale + alpha
			WA = 2,
			// red + green + blue
			RGB = 3,
			// red + green + blue + alpha
			RGBA = 4,
		};

		Vector2i size{};
		int channels{};
		unsigned char* data = nullptr;

		// fallback default texture
		static Image WHITE_PIXEL;

		Image(Channels channels, Vector2i size, unsigned char* data)
			: channels(static_cast<int>(channels)), size(size), data(data), type(ImageType_RawData) {
			ANDROMEDA_ASSERT(channels > 0);
		}
		
		Image(const std::filesystem::path& filePath, Channels channels = RGBA);

		IntRect GetFullRect() const {
			return IntRect(0, 0, size.x, size.y);
		}

		void Destroy();
	};

	struct Sprite {
		Sprite(Image image, IntRect rect) : image(image), rect(rect) {}

		const Image image;
		const IntRect rect;
	};
} // namespace andromeda