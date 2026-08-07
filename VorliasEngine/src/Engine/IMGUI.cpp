#include "Engine/IMGUI.h"
#include "Engine/Engine.h"

#define IMGUI_IMPL_VULKAN_USE_VOLK
// #define IMGUI_IMPL_VULKAN_VOLK_FILENAME "volk.h" 
#include "imgui/imgui_impl_vulkan.h"
#include "imgui/imgui_impl_sdl3.h"

namespace andromeda {
    const ImWindow& ImWindow::Initialize(Window& window) {
        ImWindow imWindow;
        imWindow.m_window = window.GetHandle();

        auto& engine = Engine::GetInstance();
        auto& renderer = engine.GetRenderer();
        
        switch (renderer->GetAPI()) {
            case graphics::Renderer::API::Vulkan:
                ImGui_ImplSDL3_InitForVulkan(window.GetHandle());
                break;
        }

        return imWindow;
    }
}