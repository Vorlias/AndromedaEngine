#include "Engine/Application.h"
#include "Engine/Log.h"

namespace andromeda {
    float Application::GetElapsedTime() const {
        return elapsedTime;
    }

    float Application::GetDeltaTime() const {
        return deltaTime;
    }

    void Application::SetFramerate(uint32_t limit) {
        if (limit <= 0) {
            frameTime = Time::Zero;
            return;
        }

        frameTime = (seconds(1.f / static_cast<float>(limit)));
        
        andromeda::print("Framerate set to " + std::to_string(limit) + "hz");
    }
}