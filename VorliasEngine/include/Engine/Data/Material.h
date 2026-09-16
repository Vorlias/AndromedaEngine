#pragma once
#include "Image.h"
#include "Color.h"

namespace andromeda {
	struct MaterialData {
		Image::SharedRef texture{};
		Color4 baseColor{};

		MaterialData(Image::SharedRef texture) : texture(texture) {}
		MaterialData(Color4 color) : baseColor(color) {}
		MaterialData(Image::SharedRef texture, Color4 color) : texture(texture), baseColor(color) {}
	};
} // namespace andromeda