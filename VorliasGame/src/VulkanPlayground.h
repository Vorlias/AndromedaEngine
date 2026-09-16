#pragma once
// #include "Engine/Graphics/Vulkan/VulkanInstance.h"

#include "Engine/Window.h"
#include "Engine/Graphics/Vulkan/VulkanRenderTexture.h"

class VulkanApplication {
public:
    VulkanApplication();
    bool Initialize();
    void Run();
    void Shutdown();

    void Update();
    void Render();
private:
    andromeda::Window window;
    andromeda::graphics::VulkanRenderTexture* rt;
};