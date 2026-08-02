#ifndef VULKAN_INST_H
#define VULKAN_INST_H
// #define VMA_IMPLEMENTATION
// #include "vk_mem_alloc.h"

#include "Engine/Graphics/GraphicsContext.h"
#include <SDL3/SDL.h>
#include <vulkan/vulkan.h>

namespace andromeda::graphics {
	class VulkanContext {
		constexpr static uint32_t VulkanVersion{VK_API_VERSION_1_4};

	public:
		bool InitVulkan();
		void Shutdown();
		inline VkInstance GetInstance() const {
			return instance;
		}

	private:
		friend class VulkanWindowContext;

		VkInstance instance = VK_NULL_HANDLE;
		bool CreateVulkanInstance();
	};
} // namespace andromeda::graphics

#endif