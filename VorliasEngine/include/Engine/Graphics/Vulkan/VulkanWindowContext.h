#pragma once
#include "Engine/Graphics/GraphicsContext.h"
#include "Engine/Graphics/Vulkan/VulkanInstance.h"
#include <SDL3/SDL.h>
#include <vulkan/vulkan.h>

struct VmaAllocator_T;
typedef struct VmaAllocator_T* VmaAllocator;
struct VmaAllocation_T;
typedef struct VmaAllocation_T* VmaAllocation;

namespace andromeda::graphics {
	class VulkanWindowContext : public GraphicsContext {
		constexpr static VkFormat swapchainFormat{VK_FORMAT_B8G8R8A8_SRGB};
		constexpr static VkFormat depthFormat{VK_FORMAT_D32_SFLOAT}; // represents a depth buffer of 32 bit floats

	public:
		VulkanWindowContext(VulkanContext& vulkan, SDL_Window* window);
		void Initialize() override;
		void Shutdown() override;
		void Resized(int width, int height) override;

		[[nodiscard]] VkDevice GetDevice() const { return device; }
	private:
		bool CreateSurface();
		bool InitializeVMA();
		bool CreateDevice(uint32_t queueIndex);
		bool CreateShaders();

		VkPhysicalDevice FindPhysicalDevice();
		uint32_t FindGraphicsQueue();

		bool CreateSwapchain(int width, int height);
		void DestroySwapchain();

		SDL_Window* window;
		VulkanContext& vulkan;

		VmaAllocator vmaAllocator = VK_NULL_HANDLE;
		VkSurfaceKHR surface = VK_NULL_HANDLE;
		VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
		VkQueue graphicsQueue = VK_NULL_HANDLE;
		VkDevice device = VK_NULL_HANDLE;

		VkImage depthImage = VK_NULL_HANDLE;
		VkImageView depthImageView = VK_NULL_HANDLE;
		VmaAllocation depthImageAllocation = VK_NULL_HANDLE;

		VkSwapchainKHR swapchain = VK_NULL_HANDLE;
		std::vector<VkImage> swapchainImages;
		std::vector<VkImageView> swapchainImageViews;
		std::vector<VkSemaphore> renderCompleteSemaphores;
		bool requireSwapchainRecreate = false;
		uint32_t swapchainWidth, swapchainHeight;
	};
} // namespace andromeda::graphics