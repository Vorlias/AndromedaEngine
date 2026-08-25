#pragma once
#include "Engine/Graphics/GraphicsContext.h"
#include "Engine/Graphics/Vulkan/VulkanShader.h"
#include "Engine/Graphics/Vulkan/VulkanInstance.h"
#include "imgui/imgui_impl_vulkan.h"
#include <SDL3/SDL.h>
#include <vulkan/vulkan.h>
#include "Engine/Graphics/Vulkan/VulkanBase.h"

#include "VulkanGraphicsPipeline.h"

namespace andromeda::graphics {
	struct FrameResources {
		VkCommandPool commandPool = nullptr;
		VkCommandBuffer commandBuffer = nullptr;
		VkSemaphore imageAcquiredSemaphore = nullptr;
	};

	class VulkanWindowContext : public GraphicsContext {
	public:
		constexpr static uint32_t MaxFramesInFlight{2};
		constexpr static VkFormat swapchainFormat{VK_FORMAT_B8G8R8A8_SRGB};
		constexpr static VkFormat depthFormat{VK_FORMAT_D32_SFLOAT}; // represents a depth buffer of 32 bit floats
	public:
		VulkanWindowContext(VulkanContext* vulkan, SDL_Window* window);

		void Initialize() override;
		void Shutdown() override;
		void Resized(int width, int height) override;

		ANDROMEDA_GETCONST VkDevice GetDevice() const {
			return device;
		}
		
		ANDROMEDA_GETCONST VkSurfaceKHR GetSurface() const {
			return surface;
		}

		ANDROMEDA_GETCONST VkSwapchainKHR GetSwapchain() const {
			return swapchain;
		}

		ANDROMEDA_GETCONST VkQueue GetGraphicsQueue() const {
			return graphicsQueue;
		}

		ANDROMEDA_GETCONST int GetSwapchainHeight() const {
			return swapchainHeight;
		}

		ANDROMEDA_GETCONST int GetSwapchainWidth() const {
			return swapchainWidth;
		}

		ANDROMEDA_GETCONST uint32_t GetMinImageCount() const {
			return m_minImageCount;
		}
		ANDROMEDA_GETCONST uint32_t GetImageCount() const {
			return m_imageCount;
		}

		ANDROMEDA_GETCONST size_t GetSemaphoreCount() const {
			return renderCompleteSemaphores.size();
		}

		ANDROMEDA_GETCONST const std::vector<VkImage>& GetSwapchainImages() const {
			return swapchainImages;
		}

		ANDROMEDA_GETCONST VkImage GetImage(size_t index) const {
			return swapchainImages[index];
		}

		ANDROMEDA_GETCONST VkImageView GetImageView(size_t index) const {
			return swapchainImageViews[index];
		}

		ANDROMEDA_GETCONST VkSemaphore GetTimelineSemaphore() const {
			return m_timelineSemaphore;
		}

		ANDROMEDA_GETCONST const std::array<FrameResources, MaxFramesInFlight>& GetFrameResources() const { 
			return m_frameResources;
		}
	private:
		[[nodiscard]] bool CreateSurface();
		[[nodiscard]] bool CreateShaders();
		// [[nodiscard]] VkPipeline CreateGraphicsPipeline();
		[[nodiscard]] bool CreateSyncResources();
		[[nodiscard]] bool CreateCommandBuffers();

		[[nodiscard]] bool CreateSwapchain(int width, int height);
		void DestroySwapchain();

		SDL_Window* window;
		VulkanContext* vulkan;

		VulkanGraphicsPipeline* m_graphicsPipeline = nullptr;

		VkSurfaceKHR surface = VK_NULL_HANDLE;

		VkQueue graphicsQueue = VK_NULL_HANDLE;
		VkDevice device = VK_NULL_HANDLE;

		VkImage depthImage = VK_NULL_HANDLE;
		VkImageView depthImageView = VK_NULL_HANDLE;
		VmaAllocation depthImageAllocation = VK_NULL_HANDLE;

		VkSwapchainKHR swapchain = VK_NULL_HANDLE;
		std::vector<VkImage> swapchainImages;
		std::vector<VkImageView> swapchainImageViews;
		std::vector<VkSemaphore> renderCompleteSemaphores;

		uint32_t swapchainWidth, swapchainHeight;

		Ref<Shader> m_shader;

		uint32_t m_minImageCount = 0, m_imageCount = 0;

		// frame and synchronization resources
		VkSemaphore m_timelineSemaphore = nullptr;
		std::array<FrameResources, MaxFramesInFlight> m_frameResources;
	};
} // namespace andromeda::graphics