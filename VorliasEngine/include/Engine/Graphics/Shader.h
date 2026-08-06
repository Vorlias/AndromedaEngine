#pragma once
#include "Engine/Common.h"

namespace andromeda::graphics {
    enum class ShaderType {
        Vertex,
        Fragment,
    };

    class Shader {
        static SharedRef<Shader> LoadShaderFromFile(const std::string& fileName, ShaderType type);
    public:

    private:
    };
}