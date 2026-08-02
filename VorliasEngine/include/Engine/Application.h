#pragma once
#include "Window.h"

namespace ENGINE_NS {
    class Application {
    public:
        virtual const WindowOptions GetWindowOptions() const {
            return WindowOptions();
        }

        virtual bool Initialize() { return true; }
        virtual void Update() {}
        virtual void Render() {}
        virtual void Shutdown() {}

        virtual void ApplicationQuit() {}
    };
}
