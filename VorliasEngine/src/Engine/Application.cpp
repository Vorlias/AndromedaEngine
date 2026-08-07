#include "Engine/Application.h"
#include "Engine/Log.h"

namespace andromeda {


    void Application::SetFramerateLimit(uint32_t limit) {
        if (limit <= 0) {
            m_frameTime = Time::Zero;
            return;
        }

        m_frameTime = (seconds(1.f / static_cast<float>(limit)));
    }

    uint32_t Application::GetFramerateLimit() const {
        return 1.f / m_frameTime.toSeconds();
    }

    void Application::Quit() {
        m_quitRequested = true;
    }
}