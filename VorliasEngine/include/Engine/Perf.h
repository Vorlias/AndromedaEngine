#pragma once
#include <chrono>
#include <thread>

namespace andromeda {
    struct Timer {
        std::chrono::time_point<std::chrono::system_clock> start, end;
        std::chrono::duration<float> duration;
        const char* name;

        Timer(const char* name): name(name) {
            start = std::chrono::high_resolution_clock::now();
        }

        ~Timer() {
            end = std::chrono::high_resolution_clock::now();
            duration = end - start;

            std::cout << "Timer " << name << " took " << duration.count() * 1000.0f << "ms " << std::endl;
        }
    };
}