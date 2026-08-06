#include "Engine/Time.h"

namespace andromeda {
	constexpr Time minutes(int32_t mins) {
		return std::chrono::minutes(mins);
	}

	constexpr Time seconds(int32_t seconds) {
		return std::chrono::seconds(seconds);
	}

	constexpr Time seconds(float seconds) {
		return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::duration<float>(seconds));
	}

	constexpr Time milliseconds(int32_t millis) {
		return std::chrono::milliseconds(millis);
	}

	constexpr Time microseconds(int64_t microsecs) {
		return std::chrono::microseconds(microsecs);
	}

	constexpr Time operator+(Time left, Time right) {
		return microseconds(left.toMicroseconds() + right.toMicroseconds());
	}

	constexpr Time operator-(Time left, Time right) {
		return microseconds(left.toMicroseconds() - right.toMicroseconds());
	}

	constexpr Time& operator+=(Time& left, Time right) {
		return left = left + right;
	}

	constexpr Time& operator-=(Time& left, Time right) {
		return left = left - right;
	}

	constexpr bool operator==(Time left, Time right) {
		return left.toMicroseconds() == right.toMicroseconds();
	}

	constexpr bool operator!=(Time left, Time right) {
		return left.toMicroseconds() != right.toMicroseconds();
	}


} // namespace andromeda