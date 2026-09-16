#pragma once
#include "imgui/imgui.h"

namespace ImGuiEx {
    void TextIcon(const char* text) {
        // auto height = ImGui::GetTextLineHeight();
        // ImGui::SetCursorPosY(ImGui::GetCursorPosY() + height / 4);
        ImGui::Text(text);
    }
} // namespace ImGuiEx