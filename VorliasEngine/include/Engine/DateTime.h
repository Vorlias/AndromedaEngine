#pragma once
#include <chrono>
#include "Engine/Time.h"

namespace andromeda {
    struct DateTime {
        DateTime() = default;
        constexpr DateTime(std::chrono::system_clock::time_point time): time(time) {}
        static constexpr DateTime now();

        constexpr Time timeSince(DateTime other) {
            auto diff = time - other.time;
            return microseconds(diff.count() / 1'000);
        }
    private:
        std::chrono::system_clock::time_point time;
    };

    [[nodiscard]] constexpr DateTime DateTime::now() {
        auto time = std::chrono::system_clock::now();
        return time;
    }
}