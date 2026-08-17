#pragma once
namespace andromeda {
	constexpr bool minf(float a, float b) {
		return (a < b) ? a : b;
	}

	constexpr bool maxf(float a, float b) {
		return (a < b) ? b : a;
	}

	constexpr bool clampf(float v, float min, float max) {
		return minf(maxf(v, min), max);
	}
} // namespace andromeda