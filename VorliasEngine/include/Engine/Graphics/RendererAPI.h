#pragma once 
#include "Engine/Data/Color.h"

namespace andromeda::graphics {
    class Renderer {
    public:
        enum class API {
            None = 0,
            Vulkan = 1,
            OpenGL = 2,
        };

        virtual bool Initialize() = 0;
        virtual void Clear() = 0;
        virtual void SetClearColor(Color color) = 0;
        virtual void Shutdown() = 0;

        virtual API GetAPI() = 0;
        virtual const std::string GetAPIString() const = 0;
    };
}