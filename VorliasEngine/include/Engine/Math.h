#pragma once
#define PI (atan(1) * 4)
#define _RADIANS(deg) ((deg) * PI / 180)
#define _DEGREES(rad) ((rad) * 180 / PI)
#include <cmath>

namespace andromeda {
	template<typename T>
		requires(std::is_integral_v<T> || std::is_floating_point_v<T>)
	constexpr T min(T a, T b) {
		return (a < b) ? a : b;
	}

	template<typename T>
		requires(std::is_integral_v<T> || std::is_floating_point_v<T>)
	constexpr T max(T a, T b) {
		return (a > b) ? a : b;
	}

	template<typename T>
		requires(std::is_integral_v<T> || std::is_floating_point_v<T>)
	constexpr T clamp(T value, T minValue, T maxValue) {
		return min<T>(max<T>(value, minValue), maxValue);
	}
} // namespace andromeda

namespace andromeda {
	constexpr float signf(float value) {
		if (value >= 0)
			return 1.0f;
		return -1.0f;
	}

	constexpr float roundf(float value) {
		return std::ceil(value + 0.5f);
	}

	constexpr float minf(float a, float b) {
		return (a < b) ? a : b;
	}

	constexpr float maxf(float a, float b) {
		return (a < b) ? b : a;
	}

	constexpr float clampf(float v, float min, float max) {
		return minf(maxf(v, min), max);
	}

	// degrees value between -360.0f and 360.0f
	constexpr float degreesClamped(float v) {
		if (v >= 360.0f) {
			return 0.0f - v;
		} else if (v <= -360) {
			return 0.0f + -v;
		}

		return v;
	}

	// degrees value between 0.0f and 360.0f, negatives wrap backwards (e.g. -90.0f is 270.0f degrees)
	constexpr float degreesClampedAbs(float v) {
		v = fmod(v, 360.0f);
		if (v < 0)
			v = 360.0f + v;

		return v;
	}

	// degrees to radians
	constexpr float degreesToRadians(float degrees) {
		return _RADIANS(degrees);
	}

	/// radians to degrees
	constexpr float radiansToDegrees(float radians) {
		return degreesClamped(_DEGREES(radians));
	}

	// radians to absolute degrees
	constexpr float radiansToDegreesAbs(float radians) {
		return degreesClampedAbs(radiansToDegrees(radians));
	}

} // namespace andromeda