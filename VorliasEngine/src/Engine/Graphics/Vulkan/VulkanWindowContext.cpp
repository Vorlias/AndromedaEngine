#include "Engine/Graphics/Vulkan/VulkanWindowContext.h"
#include "Engine/Log.h"
#include <SDL3/SDL_vulkan.h>


namespace andromeda::graphics {
	VulkanWindowContext::VulkanWindowContext(VulkanContext& vulkan, SDL_Window* window) : vulkan(vulkan) {}

	void VulkanWindowContext::Initialize() {
	}

	void VulkanWindowContext::Shutdown() {}
} // namespace andromeda::graphics