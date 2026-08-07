#pragma once
#include <math.h>

namespace andromeda {
	struct Vector3;
	struct Vector2 {
		float x, y;

		Vector2() = default;
		Vector2(float x, float y) : x(x), y(y) {}

		float GetMagnitude();
		float GetDistance(Vector2 other);
		Vector3 Extend(float z = 0);
	};

	struct Vector3 {
		float x, y, z;

		Vector3() = default;
		Vector3(float x, float y, float z) : x(x), y(y), z(z) {}

		float GetMagnitude();
		float GetDistance(Vector3 other);

		Vector2 Shrink();
	};

    // two dimensional unsigned-based vector
	struct Vector2u {
		uint32_t x, y;

		Vector2u() = default;
		Vector2u(uint32_t x, uint32_t y) : x(x), y(y) {}
	};

    // two dimensional integer-based vector
	struct Vector2i {
		int32_t x, y;

		Vector2i() = default;
		Vector2i(int32_t x, int32_t y) : x(x), y(y) {}
	};

    // three dimensional integer-based vector
	struct Vector3i {
		int32_t x, y, z;

		Vector3i() = default;
		Vector3i(int32_t x, int32_t y, int32_t z) : x(x), y(y), z(z) {}
	};

	inline std::string to_string(const Vector2& vec) {
		return std::format("<{:.2f}f, {:.2f}f>", vec.x, vec.y);
	}

	inline std::string to_string(const Vector2u& vec) {
		return std::format("<{:d}u, {:d}u>", vec.x, vec.y);
	}

	inline std::string to_string(const Vector2i& vec) {
		return std::format("<{:d}, {:d}>", vec.x, vec.y);
	}

	inline std::string to_string(const Vector3i& vec) {
		return std::format("<{:d}, {:d}, {:d}>", vec.x, vec.y, vec.z);
	}
} // namespace andromeda