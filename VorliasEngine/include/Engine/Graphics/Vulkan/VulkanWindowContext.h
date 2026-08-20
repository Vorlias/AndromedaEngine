#pragma once
#include "Engine/Graphics/GraphicsContext.h"
#include "Engine/Graphics/Vulkan/VulkanShader.h"
#include "Engine/Graphics/Vulkan/VulkanInstance.h"
#include "imgui/imgui_impl_vulkan.h"
#include <SDL3/SDL.h>
#include <vulkan/vulkan.h>
#include "Engine/Graphics/Vulkan/VulkanBase.h"

// struct VmaAllocator_T;
// typedef struct VmaAllocator_T* VmaAllocator;
// struct VmaAllocation_T;
// typedef struct VmaAllocation_T* VmaAllocation;

namespace andromeda::graphics {
	class VulkanWindowContext : public GraphicsContext {
	public:
		constexpr static VkFormat swapchainFormat{VK_FORMAT_B8G8R8A8_SRGB};
		constexpr static VkFormat depthFormat{VK_FORMAT_D32_SFLOAT}; // represents a depth buffer of 32 bit floats
	public:
		VulkanWindowContext(VulkanContext* vulkan, SDL_Window* window);

		void Initialize() override;
		void Shutdown() override;
		void Resized(int width, int height) override;

		ANDROMEDA_GETCONST VkDevice GetDevice() const { return device; }
		ANDROMEDA_GETCONST VkSurfaceKHR GetSurface() const { return surface; }
		ANDROMEDA_GETCONST VkSwapchainKHR GetSwapchain() const { return swapchain; }
		ANDROMEDA_GETCONST VkQueue GetGraphicsQueue() const { return graphicsQueue; }
		ANDROMEDA_GETCONST int GetSwapchainHeight() const { return swapchainHeight; }
		ANDROMEDA_GETCONST int GetSwapchainWidth() const { return swapchainWidth; }

		ANDROMEDA_GETCONST uint32_t GetMinImageCount() const { return m_minImageCount; }
		ANDROMEDA_GETCONST uint32_t GetImageCount() const { return m_imageCount; }

		ANDROMEDA_GETCONST size_t GetSemaphoreCount() const { return renderCompleteSemaphores.size(); }
		ANDROMEDA_GETCONST const std::vector<VkImage>& GetSwapchainImages() const { return swapchainImages; }
		ANDROMEDA_GETCONST VkImage GetImage(size_t index) const { return swapchainImages[index]; }
		ANDROMEDA_GETCONST VkImageView GetImageView(size_t index) const { return swapchainImageViews[index]; }

		void CreateCommandBuffers(uint32_t imageCount, VkCommandBuffer* buffers) {
			// TODO:
		}

		void SetupIMGUI(ImGui_ImplVulkanH_Window* wd);
	private:
		[[nodiscard]] bool CreateSurface();
		[[nodiscard]] bool CreateShaders();
		[[nodiscard]] VkPipeline CreateGraphicsPipeline();

		[[nodiscard]] bool CreateSwapchain(int width, int height);
		void DestroySwapchain();

		SDL_Window* window;
		VulkanContext* vulkan;

		VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
		VkPipeline pipeline = VK_NULL_HANDLE;

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

		VkPipeline m_pipeline = VK_NULL_HANDLE;

		uint32_t m_minImageCount = 0, m_imageCount = 0;
	};
} // namespace andromeda::graphics