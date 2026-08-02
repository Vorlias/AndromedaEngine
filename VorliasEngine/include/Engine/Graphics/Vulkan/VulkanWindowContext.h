#pragma once
#include "Engine/Graphics/GraphicsContext.h"
#include "Engine/Graphics/Vulkan/VulkanInstance.h"
#include <SDL3/SDL.h>
#include <vulkan/vulkan.h>

namespace andromeda::graphics {
	class VulkanWindowContext : public GraphicsContext {
		constexpr static VkFormat swapchainFormat{VK_FORMAT_B8G8R8A8_SRGB};

	public:
		VulkanWindowContext(VulkanContext& vulkan, SDL_Window* window);
		void Initialize() override;
		void Shutdown() override;

	private:
		SDL_Window* window;

		VulkanContext& vulkan;
		VkSurfaceKHR surface = VK_NULL_HANDLE;
		VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
		VkQueue graphicsQueue = VK_NULL_HANDLE;
		VkDevice device = VK_NULL_HANDLE;
		VkSwapchainKHR swapchain = VK_NULL_HANDLE;
		uint32_t swapchainWidth, swapchainHeight;
	};
} // namespace andromeda::graphics