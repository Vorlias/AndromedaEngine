#pragma once
#include <chrono>
#include <ratio>

namespace andromeda {
	class Time {
	public:
		constexpr Time() = default;
		template<typename Rep, typename Period>
		constexpr Time(const std::chrono::duration<Rep, Period>& duration) : m_microseconds(duration) {}

		// constexpr Time& operator=(const Time& time) {
		//     m_microseconds = time.m_microseconds;
		// }

		constexpr int64_t toMicroseconds() const {
			return m_microseconds.count();
		}

		constexpr int32_t toMilliseconds() const {
			return std::chrono::duration_cast<std::chrono::duration<std::int32_t, std::milli>>(m_microseconds).count();
		}

		constexpr float toSeconds() const {
			return std::chrono::duration<float>(m_microseconds).count();
		}

		static Time Zero;

	private:
		std::chrono::microseconds m_microseconds{};
	};

	[[nodiscard]] constexpr Time minutes(int32_t mins);
	[[nodiscard]] constexpr Time seconds(int32_t secs);
	[[nodiscard]] constexpr Time seconds(float secs);
	[[nodiscard]] constexpr Time milliseconds(int32_t msecs);
	[[nodiscard]] constexpr Time microseconds(int64_t usecs);

	[[nodiscard]] constexpr Time operator+(Time left, Time right);
	[[nodiscard]] constexpr Time operator-(Time left, Time right);

	[[nodiscard]] constexpr bool operator==(Time left, Time right);
	[[nodiscard]] constexpr bool operator!=(Time left, Time right);

	constexpr Time& operator+=(Time& left, Time right);
	constexpr Time& operator-=(Time& left, Time right);

	// Sleep for a given amount of time, internally uses SDL_Delay
	void sleep(Time time);
} // namespace andromeda

#include "Time.inl"