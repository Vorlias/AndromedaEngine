#include "Engine/Graphics/Vulkan/VulkanRendererAPI.h"

namespace andromeda::graphics {
    bool VulkanRenderer::Initialize() {
        if (!context.InitVulkan()) return false;
        return true;
    }

    void VulkanRenderer::Clear() {}

    void VulkanRenderer::SetClearColor(Color color) {}

    void VulkanRenderer::Shutdown() {
        context.Shutdown();
    }
}

const std::string andromeda::graphics::VulkanRenderer::GetAPIString() const {
	return std::string("Vulkan");
}
