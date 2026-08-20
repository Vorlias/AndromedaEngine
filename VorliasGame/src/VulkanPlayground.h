#pragma once
// #include "Engine/Graphics/Vulkan/VulkanInstance.h"

#include "Engine/Window.h"


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
};