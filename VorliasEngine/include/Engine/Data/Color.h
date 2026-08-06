#pragma once

namespace andromeda {
	// 16 bytes
	struct Color {
		float r{1}, g{1}, b{1}, a{1};

		Color() = default;
		Color(float r, float g, float b) : r(r), g(g), b(b), a(1) {}
		Color(float r, float g, float b, float a) : r(r), g(g), b(b), a(a) {}

		static Color FromRGB(byte r, byte g, byte b) {
			return Color(r / 255.f, g / 255.f, b / 255.f);
		}

		operator float*() const {
			return (float*)&r;
		}
	};

	// 4 bytes
	struct Color32 {
		uint8_t r{255}, g{255}, b{255}, a{255};

		Color32() = default;
		Color32(uint8_t r, uint8_t g, uint8_t b) : r(r), g(g), b(b), a(255) {}
		Color32(uint8_t r, uint8_t g, uint8_t b, uint8_t a) : r(r), g(g), b(b), a(a) {}

		operator Color() const {
			return Color(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
		}
	};
} // namespace andromeda