#include "Engine/Graphics/Vulkan/VulkanWindowContext.h"
#include "Engine/Graphics/Vulkan/VulkanUtils.h"
#include "Engine/Log.h"


#include "Engine/Graphics/Vulkan/VulkanBase.h"

#include <SDL3/SDL_vulkan.h>
#include <spdlog/spdlog.h>
#include "Engine/Graphics/Vulkan/VulkanShader.h"

namespace andromeda::graphics {
	VulkanWindowContext::VulkanWindowContext(VulkanContext* vulkan, SDL_Window* window) : vulkan(vulkan), window(window) {
		andromeda::trace("Create window context");
	}

	void VulkanWindowContext::Initialize() {
		if (!CreateSurface())
			return;

		device = vulkan->GetDevice();

		int width, height;
		SDL_GetWindowSize(window, &width, &height);
		if (!CreateSwapchain(width, height)) {
			return;
		}

		if (!CreateShaders())
			return;

		m_graphicsPipeline = new VulkanGraphicsPipeline(vulkan, swapchainFormat, depthFormat, static_cast<VulkanShader*>(m_shader.AsPtr()));
		if (!m_graphicsPipeline->Create()) {
			return;
		}

		if (!CreateSyncResources()) {
			return;
		}

		if (!CreateCommandBuffers()) {
			andromeda::error("Could not create command buffers");
			return;
		}
	}

	void VulkanWindowContext::Resized(int width, int height) {
		DestroySwapchain();
		CreateSwapchain(width, height);
	}

	bool VulkanWindowContext::CreateSurface() {
		if (!SDL_Vulkan_CreateSurface(window, vulkan->GetInstance(), nullptr, &surface)) {
			andromeda::warn("Could not create surface for window " + std::to_string(SDL_GetWindowID(window)));
			return false;
		}

		andromeda::trace("Create surface for window");
		return true;
	}

	bool VulkanWindowContext::CreateSwapchain(int width, int height) {
		// Track swapchain size separate from window size
		swapchainWidth = width;
		swapchainHeight = height;

		// Ensure tracking apropriate number of images
		VkSurfaceCapabilitiesKHR surfaceCaps{};
		if (vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vulkan->GetPhysicalDevice(), surface, &surfaceCaps) != VK_SUCCESS) {
			andromeda::error("Could not get the surface capabilities");
			return false;
		}

		uint32_t requestedImageCount = std::max(2u, surfaceCaps.minImageCount);
		if (surfaceCaps.maxImageCount > 0) {
			requestedImageCount = std::min(requestedImageCount, surfaceCaps.maxImageCount);
		}

		m_minImageCount = surfaceCaps.minImageCount;

		VkSwapchainCreateInfoKHR swapchainCreateInfo{
			.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
			.surface = surface,
			.minImageCount = requestedImageCount,
			.imageFormat = swapchainFormat,
			.imageColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR, // makes sure colors are reproduced correctly
			.imageExtent{.width = swapchainWidth, .height = swapchainHeight}, // the dimensions of the images
			.imageArrayLayers = 1, // each swapchain image should be a single image
			.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, // we're just drawing
			.preTransform = surfaceCaps.currentTransform, // how to orient the image - we account for any custom user rotations etc.
			.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR, // ensure window is fully opaque, allowing transparency
			.presentMode = VK_PRESENT_MODE_FIFO_KHR, // how the presentation engine chooses which image to draw - standard guarantee is surface will
		                                             // have this and good w/ vsync
		};

		if (vkCreateSwapchainKHR(device, &swapchainCreateInfo, nullptr, &swapchain) != VK_SUCCESS) {
			return false;
		}

		// ask for the swapchain images
		uint32_t imageCount = 0;
		vkGetSwapchainImagesKHR(device, swapchain, &imageCount, nullptr);
		swapchainImages.resize(imageCount);
		vkGetSwapchainImagesKHR(device, swapchain, &imageCount, swapchainImages.data());
		swapchainImageViews.resize(imageCount);

		andromeda::trace("Created swapchain images count=" + std::to_string(imageCount));
		m_imageCount = imageCount;

		for (size_t i = 0; i < swapchainImages.size(); i++) {
			VkImageViewCreateInfo imgViewInfo{
				.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
				.image = swapchainImages[i],
				.viewType = VK_IMAGE_VIEW_TYPE_2D, // regular 2D image
				.format = swapchainFormat,
				.subresourceRange{
					// dictates what portion of the image to access
					.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
					.baseMipLevel = 0,
					.levelCount = 1, // mipMapLevelCount
					.baseArrayLayer = 0,
					.layerCount = 1,
				},
			};

			if (vkCreateImageView(device, &imgViewInfo, nullptr, &swapchainImageViews[i]) != VK_SUCCESS) {
				andromeda::error("Failed to create image view");
				return false;
			}
		}

		renderCompleteSemaphores.resize(swapchainImages.size());
		for (VkSemaphore& semaphore : renderCompleteSemaphores) {
			VkSemaphoreCreateInfo semaphoreInfo{.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};

			if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &semaphore) != VK_SUCCESS) {
				andromeda::error("Error creating the render-complete semaphore");
				return false;
			}
		}


		// swapchain images owned by OS, depth has to be created

		// Create depth image - ensure visibility order is maintained
		VkImageCreateInfo depthCreateInfo{
			.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
			.imageType = VK_IMAGE_TYPE_2D,
			.format = depthFormat,
			.extent{.width = swapchainWidth, .height = swapchainHeight, .depth = 1},
			.mipLevels = 1,
			.arrayLayers = 1, // single image depth buffer
			.samples = VK_SAMPLE_COUNT_1_BIT, // don't need to multisample
			.tiling = VK_IMAGE_TILING_OPTIMAL,
			.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, // depth or stencil operations
			.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED, // let vulkan know we don't care looking at memory that is init alloc
		};

		// Let VMA allocate the image
		VmaAllocationCreateInfo allocInfo{
			.flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT, // chunk of data for dedicated for the depth buffer
			.usage = VMA_MEMORY_USAGE_AUTO, // vma determines what to do with it
		};

		if (vmaCreateImage(vulkan->GetAllocator(), &depthCreateInfo, &allocInfo, &depthImage, &depthImageAllocation, nullptr) != VK_SUCCESS) {
			andromeda::error("Failed to allocate depth image");
			return false;
		}


		VkImageViewCreateInfo depthImgViewInfo{
			.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
			.image = depthImage,
			.viewType = VK_IMAGE_VIEW_TYPE_2D,
			.format = depthFormat,
			.subresourceRange{.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT, .levelCount = 1, .layerCount = 1}, // only depth data (only aspect)
		};

		if (vkCreateImageView(device, &depthImgViewInfo, nullptr, &depthImageView) != VK_SUCCESS) {
			andromeda::error("Error creating depth image view");
			return false;
		}

		return true;
	}

	void VulkanWindowContext::DestroySwapchain() {
		for (VkImageView swapchainImageView : swapchainImageViews) {
			vkDestroyImageView(device, swapchainImageView, nullptr);
		}
		swapchainImageViews.clear();

		for (VkSemaphore& semaphore : renderCompleteSemaphores) {
			vkDestroySemaphore(device, semaphore, nullptr);
		}
		renderCompleteSemaphores.clear();

		if (swapchain) {
			vkDestroySwapchainKHR(device, swapchain, nullptr);
			swapchain = nullptr;
		}

		if (depthImageView) {
			vkDestroyImageView(device, depthImageView, nullptr);
			vmaDestroyImage(vulkan->GetAllocator(), depthImage, depthImageAllocation);
			depthImageView = nullptr;
		}
	}

	bool VulkanWindowContext::CreateShaders() {
		VulkanShader* vertShader = new VulkanShader(device);
		vertShader->LoadFromFile("VorliasEngine/src/Shaders/shader.vert", ShaderType::Vertex);
		vertShader->LoadFromFile("VorliasEngine/src/Shaders/shader.frag", ShaderType::Fragment);
		m_shader = vertShader;
		return true;
	}

	bool VulkanWindowContext::CreateSyncResources() {
		// synchronize frames in flight
		VkSemaphoreTypeCreateInfo semaphoreTypeInfo{
			.sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO,
			.semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE,
			.initialValue = MaxFramesInFlight,
		};
		VkSemaphoreCreateInfo semaphoreInfo{
			.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
			.pNext = &semaphoreTypeInfo,
		};

		if (vkCreateSemaphore(vulkan->GetDevice(), &semaphoreInfo, nullptr, &m_timelineSemaphore) != VK_SUCCESS) {
			andromeda::error("Failed to create timeline semaphore");
			return false;
		}


		// per frame image acquire semaphores
		for (FrameResources& res : m_frameResources) {
			// create the binary semaphores
			VkSemaphoreCreateInfo semaphoreInfo{.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
			if (vkCreateSemaphore(vulkan->GetDevice(), &semaphoreInfo, nullptr, &res.imageAcquiredSemaphore) != VK_SUCCESS) {
				andromeda::error("Error creating the per-frame image-acquire semaphore");
				return false;
			}
		}

		return true;
	}

	bool VulkanWindowContext::CreateCommandBuffers() {
		for (FrameResources& res : m_frameResources) {
			// Give ecah frame it's own pool, faster cmd buffer resets this way
			VkCommandPoolCreateInfo poolInfo{
				.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
				.queueFamilyIndex = static_cast<uint32_t>(vulkan->GetGraphicsFamilyIndex()),
			};

			if (vkCreateCommandPool(vulkan->GetDevice(), &poolInfo, nullptr, &res.commandPool) != VK_SUCCESS) {
				andromeda::error("Failed to create command buffer pool");
				return false;
			}

			// Create the command buffer for this frame
			VkCommandBufferAllocateInfo cmdAllocInfo{
				.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
				.commandPool = res.commandPool,
				.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
				.commandBufferCount = 1,
			};

			if (vkAllocateCommandBuffers(vulkan->GetDevice(), &cmdAllocInfo, &res.commandBuffer) != VK_SUCCESS) {
				andromeda::error("Failed to allocate command buffer");
				return false;
			}
		}

		return true;
	}

	void VulkanWindowContext::Shutdown() {
		m_shader->Unload();
		m_shader.Reset();

		for (FrameResources& res : m_frameResources) {
			if (res.imageAcquiredSemaphore != nullptr)
				vkDestroySemaphore(vulkan->GetDevice(), res.imageAcquiredSemaphore, nullptr);

			if (res.commandPool != nullptr)
				vkDestroyCommandPool(vulkan->GetDevice(), res.commandPool, nullptr);
		}

		if (m_timelineSemaphore != nullptr) {
			vkDestroySemaphore(vulkan->GetDevice(), m_timelineSemaphore, nullptr);
		}

		if (m_graphicsPipeline != nullptr) {
			delete m_graphicsPipeline;
		}

		DestroySwapchain();

		if (surface != VK_NULL_HANDLE) {
			andromeda::trace("Cleaned up surface");
			SDL_Vulkan_DestroySurface(vulkan->GetInstance(), surface, nullptr);
			surface = nullptr;
		}
	}
} // namespace andromeda::graphics