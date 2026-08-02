#include "Engine/Graphics/Vulkan/VulkanRendererAPI.h"
#include "Engine/Graphics/Vulkan/VulkanWindowContext.h"
#include "Engine/Log.h"

namespace andromeda::graphics {
	bool VulkanRenderer::Initialize() {
		if (!context.InitVulkan())
			return false;
		return true;
	}

	void VulkanRenderer::Clear() {}

	void VulkanRenderer::SetClearColor(Color color) {}

	void VulkanRenderer::Shutdown() {
		context.Shutdown();
	}
} // namespace andromeda::graphics

const std::string andromeda::graphics::VulkanRenderer::GetAPIString() const {
	return std::string("Vulkan") + " " + std::to_string(VK_VERSION_MAJOR(VulkanContext::VulkanVersion)) + "." +
	       std::to_string(VK_VERSION_MINOR(VulkanContext::VulkanVersion));
}

andromeda::graphics::GraphicsContext* andromeda::graphics::VulkanRenderer::CreateWindowGraphicsContext(SDL_Window* window) {
	return new VulkanWindowContext(context, window);
}


andromeda::ScopeRef<andromeda::graphics::GraphicsContext> andromeda::graphics::VulkanRenderer::CreateGraphicsContext(SDL_Window* window) {
    return CreateScopeRef<andromeda::graphics::VulkanWindowContext>(context, window);
}

andromeda::graphics::VulkanRenderer::~VulkanRenderer() {
    this->Shutdown();
}