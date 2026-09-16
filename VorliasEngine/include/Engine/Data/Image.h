#pragma once
#include "Vector.h"
#include <filesystem>
#include "SDL3/SDL.h"
#include "Rect.h"

namespace andromeda {
	struct Image {
		// texture id assigned by the engine
		uint32_t textureId{};

		enum ImageType {
			ImageType_RawData,
			ImageType_STBI,
		} type;

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

		Image() = default;
		Image(Channels channels, Vector2i size, unsigned char* data)
			: channels(static_cast<int>(channels)), size(size), data(data), type(ImageType_RawData) {
			ANDROMEDA_ASSERT(channels > 0);
		}

		IntRect GetFullRect() const {
			return IntRect(0, 0, size.x, size.y);
		}

		void LoadFile(const std::filesystem::path& path, Channels channels = RGBA);
		void Destroy();

		typedef void (*HandleDtor)(Image* ptr);

		typedef std::unique_ptr<Image, HandleDtor> UniqueRef;
		typedef std::shared_ptr<Image> SharedRef;

		static UniqueRef LoadFromFile(const std::filesystem::path& path, Channels channels = RGBA) {
			return std::unique_ptr<Image, HandleDtor>(new Image(path, channels), [](Image* ptr) {
				ptr->Destroy();
				delete ptr;
			});
		}

		static UniqueRef LoadFromMemory(Channels channels, Vector2i size, unsigned char* data) {
			return std::unique_ptr<Image, HandleDtor>(new Image(channels, size, data), [](Image* ptr) {
				delete ptr;
			});
		}

		static SharedRef LoadSharedImageFromFile(const std::filesystem::path& path, Channels channels = RGBA) {
			return std::shared_ptr<Image>(new Image(path, channels), [](Image* ptr) {
				ptr->Destroy();
				delete ptr;
			});
		}

	private:
		Image(const std::filesystem::path& filePath, Channels channels = RGBA);
	};

	struct Sprite {
		Sprite(const Image& image, IntRect rect) : image(image), rect(rect) {}

		const Image& image;
		const IntRect rect;
	};
} // namespace andromeda