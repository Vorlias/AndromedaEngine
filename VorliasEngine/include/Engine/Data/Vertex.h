#pragma once
#include "Vector.h"
#include "Color.h"

namespace andromeda {
    struct Vertex {
        Vector3 position;
        Color3 color;
        Vector3 normal;
        Vector2 uv;
    };
}