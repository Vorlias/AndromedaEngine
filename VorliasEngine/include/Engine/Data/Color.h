#pragma once

namespace andromeda {
	// 16 bytes
	struct Color4 {
		float r{1}, g{1}, b{1}, a{1};

		Color4() = default;
		Color4(float r, float g, float b) : r(r), g(g), b(b), a(1) {}
		Color4(float r, float g, float b, float a) : r(r), g(g), b(b), a(a) {}

		static Color4 FromRGB(byte r, byte g, byte b) {
			return Color4(r / 255.f, g / 255.f, b / 255.f);
		}

		operator float*() const {
			return (float*)&r;
		}
	};

	using Color = Color4;

	struct Color3 {
		float r{1}, g{1}, b{1};

		Color3() = default;
		Color3(float r, float g, float b) : r(r), g(g), b(b) {}

		static Color3 FromRGB(byte r, byte g, byte b) {
			return Color3(r / 255.f, g / 255.f, b / 255.f);
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
	
	inline std::string to_string(const Color& color) {
		return std::format("rgba({}, {}, {}, {})", color.r, color.g, color.b, color.a);
	}
} // namespace andromeda