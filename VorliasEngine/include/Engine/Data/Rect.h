#pragma once
#include "Vector.h"
#include "Engine/Math.h"

namespace andromeda {
	struct Rect;
	struct IntRect {
		Vector2i size;
		Vector2i position;
		IntRect(Vector2i size, Vector2i position) : size(size), position(position) {}
		IntRect(int32_t left, int32_t top, int32_t width, int32_t height) : size(width, height), position(left, top) {}

		[[nodiscard]] constexpr int32_t GetX() const {
			return position.x;
		}
		[[nodiscard]] constexpr int32_t GetY() const {
			return position.y;
		}
		[[nodiscard]] constexpr int32_t GetWidth() const {
			return size.x;
		}
		[[nodiscard]] constexpr int32_t GetHeight() const {
			return size.y;
		}

		[[nodiscard]] const Vector2i GetCenter() const {
			return position + (size / 2);
		}
	};

	struct Rect {
	public:
		Rect(Vector2 size, Vector2 position) : size(size), position(position) {}
		Rect(float left, float top, float width, float height) : size(width, height), position(left, top) {}

		Rect(const IntRect& intRect)
			: size(static_cast<float>(intRect.size.x), static_cast<float>(intRect.size.y))
			, position(static_cast<float>(intRect.position.x), static_cast<float>(intRect.position.y)) {}

		Vector2 size;
		Vector2 position;

		[[nodiscard]] constexpr float GetX() const {
			return position.x;
		}
		[[nodiscard]] constexpr float GetY() const {
			return position.y;
		}
		[[nodiscard]] constexpr float GetWidth() const {
			return size.x;
		}
		[[nodiscard]] constexpr float GetHeight() const {
			return size.y;
		}

		[[nodiscard]] bool Contains(const Vector2& point) const {
			const float minX = minf(position.x, position.x + size.x);
			const float maxX = maxf(position.x, position.x + size.x);

			const float minY = minf(position.y, position.y + size.y);
			const float maxY = maxf(position.y, position.y + size.y);

			return (point.x >= minX) && (point.x < maxX) && (point.y >= minY) && (point.y < maxY);
		}

		[[nodiscard]] const Vector2 GetCenter() const {
			return position + (size / 2.f);
		}
	};

	inline std::string to_string(const Rect& rect) {
		return "Rect(" + to_string(rect.position) + ", " + to_string(rect.size) +")";
	}

	inline std::string to_string(const IntRect& rect) {
		return "IntRect(" + to_string(rect.position) + ", " + to_string(rect.size) +")";
	}
} // namespace andromeda
