#pragma once
#include "Vector.h"
#include "Engine/Math.h"

namespace andromeda {
	struct Rect {
	public:
		Rect(Vector2 size, Vector2 position) : size(size), position(position) {}
		Rect(float left, float top, float width, float height) : size(width, height), position(left, top) {}

		Vector2 size;
		Vector2 position;

		[[nodiscard]] constexpr float GetX() {
			return position.x;
		}
		[[nodiscard]] constexpr float GetY() {
			return position.y;
		}
		[[nodiscard]] constexpr float GetWidth() {
			return size.x;
		}
		[[nodiscard]] constexpr float GetHeight() {
			return size.y;
		}

		bool Contains(const Vector2& point) {
			const float minX = minf(position.x, position.x + size.x);
			const float maxX = maxf(position.x, position.x + size.x);

			const float minY = minf(position.y, position.y + size.y);
			const float maxY = maxf(position.y, position.y + size.y);

			return (point.x >= minX) && (point.x < maxX) && (point.y >= minY) && (point.y < maxY);
		}

		const Vector2 GetCenter() const {
			return position + (size / 2.f);
		}
	};

	inline std::string to_string(const Rect& rect) {
		return "rect";
	}
} // namespace andromeda