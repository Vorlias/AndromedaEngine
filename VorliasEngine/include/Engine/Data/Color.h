#pragma once

namespace andromeda {
    struct Color {
        float r, g, b;

        Color() = default;
        Color(float r, float g, float b): r(r), g(g), b(b) {}

        static Color FromRGB(byte r, byte g, byte b) {
            return Color(r / 255.f, g / 255.f, b / 255.f);
        }
    };
}