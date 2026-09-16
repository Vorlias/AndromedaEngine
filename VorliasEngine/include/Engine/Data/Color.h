#pragma once
#include "Vector.h"

namespace andromeda {
	struct Color3;

	// RGBA in float format (16 bytes)
	struct Color4 {
		static constexpr uint32_t elements = 4;
		static constexpr size_t bytes = elements * sizeof(float);

		float r{1}, g{1}, b{1}, a{1};

		Color4() = default;
		Color4(float r, float g, float b) : r(r), g(g), b(b), a(1) {}
		Color4(float r, float g, float b, float a) : r(r), g(g), b(b), a(a) {}
		explicit Color4(uint32_t rgba) {
			r = ((rgba & 0xff000000) >> 24) / 255.f;
			g = ((rgba & 0x00ff0000) >> 16) / 255.f;
			b = ((rgba & 0x0000ff00) >> 8) / 255.f;
			a = ((rgba & 0x000000ff) >> 0) / 255.f;
		}

		// std::array<float, 4> ToArray() {
		// 	return std::array{r, g, b, a};
		// }

		static Color4 FromRGB(byte r, byte g, byte b) {
			return Color4(r / 255.f, g / 255.f, b / 255.f);
		}

		static Color4 FromRGBA(byte r, byte g, byte b, byte a) {
			return Color4(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
		}

		static Color4 FromHex(std::string_view str) {
			uint32_t value;
			std::stringstream ss;
			ss << std::hex << str;
			ss >> value;
			return Color4(value);
		}

		operator Color3() const;
		operator uint32_t() const;
	};

	using Color = Color4;

	// RGB in float format (12 bytes)
	struct Color3 {
		static constexpr uint32_t elements = 3;
		static constexpr size_t bytes = elements * sizeof(float);

		float r{1}, g{1}, b{1};

		Color3() = default;
		Color3(float r, float g, float b) : r(r), g(g), b(b) {}
		explicit Color3(uint32_t rgb) {
			if (rgb > 0xFF'FF'FF) { // assume RGBA
				r = ((rgb & 0xff000000) >> 24) / 255.f;
				g = ((rgb & 0x00ff0000) >> 16) / 255.f;
				b = ((rgb & 0x0000ff00) >> 8) / 255.f;

				auto a = ((rgb & 0x000000ff) >> 0) / 255.f;
				r *= a;
				g *= a;
				b *= a;
			} else {
				r = ((rgb & 0xff0000) >> 16) / 255.f;
				g = ((rgb & 0x00ff00) >> 8) / 255.f;
				b = ((rgb & 0x0000ff) >> 0) / 255.f;
			}
		}

		// std::array<float, 3> ToArray() {
		// 	return std::array{r, g, b};
		// }

		static Color3 FromRGB(byte r, byte g, byte b) {
			return Color3(r / 255.f, g / 255.f, b / 255.f);
		}

		static Color3 FromHex(std::string_view str) {
			uint32_t value;
			std::stringstream ss;
			ss << std::hex << str;
			ss >> value;
			return Color3(value);
		}

		operator Color4() const;
		Color4 WithAlpha(float a) const;

		operator uint32_t() const;

		explicit operator Vector3() const;
	};

	// RGBA represented in bytes (4 bytes)
	struct Color4i {
		static constexpr uint32_t elements = 4;
		static constexpr size_t bytes = elements * sizeof(uint8_t);

		uint8_t r{255}, g{255}, b{255}, a{255};

		Color4i() = default;
		Color4i(uint8_t r, uint8_t g, uint8_t b) : r(r), g(g), b(b), a(255) {}
		Color4i(uint8_t r, uint8_t g, uint8_t b, uint8_t a) : r(r), g(g), b(b), a(a) {}

		operator Color4() const {
			return Color4(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
		}
	};

	// RGB represented in bytes (3 bytes)
	struct Color3i {
		static constexpr uint32_t elements = 3;
		static constexpr size_t bytes = elements * sizeof(uint8_t);

		uint8_t r{255}, g{255}, b{255};

		Color3i() = default;
		Color3i(uint8_t r, uint8_t g, uint8_t b) : r(r), g(g), b(b) {}

		operator Color3() const {
			return Color3(r / 255.f, g / 255.f, b / 255.f);
		}
	};

	enum class ColorFormatStyle {
		Values,
		Hexidecimal,
	};

	inline std::string to_string(const Color4& color, ColorFormatStyle format = ColorFormatStyle::Values) {
		switch (format) {
			using enum ColorFormatStyle;
			case Values:
				return std::format("Color4({:.2f}, {:.2f}, {:.2f}, {:.2f})", color.r, color.g, color.b, color.a);
			case Hexidecimal: {
				uint8_t r = color.r * 255;
				uint8_t g = color.g * 255;
				uint8_t b = color.b * 255;
				uint8_t a = color.a * 255;
				return std::format("{:02X}{:02X}{:02X}{:02X}", r, g, b, a);
			}
		}
	}

	inline std::string to_string(const Color3& color, ColorFormatStyle format = ColorFormatStyle::Values) {
		switch (format) {
			using enum ColorFormatStyle;
			case Values:
				return std::format("Color3({:.2f}, {:.2f}, {:.2f})", color.r, color.g, color.b);
			case Hexidecimal: {
				uint8_t r = color.r * 255;
				uint8_t g = color.g * 255;
				uint8_t b = color.b * 255;
				return std::format("{:02X}{:02X}{:02X}", r, g, b);
			}
		}
	}
} // namespace andromeda

inline andromeda::Color4::operator Color3() const {
	return Color3(r, g, b);
}


inline andromeda::Color4::operator uint32_t() const {
	uint8_t R = r * 255;
	uint8_t G = g * 255;
	uint8_t B = b * 255;
	uint8_t A = a * 255;
	return ((R << 24) | (G << 16) | (B << 8) | A);
}

inline andromeda::Color4 andromeda::Color3::WithAlpha(float a) const {
	return Color4(r, g, b, a);
}

inline andromeda::Color3::operator Color4() const {
	return Color4(r, g, b, 1);
}

inline andromeda::Color3::operator uint32_t() const {
	uint8_t R = r * 255;
	uint8_t G = g * 255;
	uint8_t B = b * 255;
	return ((R << 16) | (G << 8) | B);
}

inline andromeda::Color3::operator andromeda::Vector3() const {
	return Vector3(r, g, b);
}