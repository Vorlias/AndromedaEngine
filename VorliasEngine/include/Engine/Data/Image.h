#pragma once
#include "Vector.h"

namespace andromeda {
	struct Image {
        enum Channels {
            RGB = 3,
            RGBA = 4,
        };

        Vector2i size;
		int channels;
		unsigned char* data;
	
        // fallback default texture
        static Image WHITE_PIXEL;
        
        Image(Channels channels, Vector2i size, unsigned char* data): channels(static_cast<int>(channels)), size(size), data(data) {}
    };
} // namespace andromeda