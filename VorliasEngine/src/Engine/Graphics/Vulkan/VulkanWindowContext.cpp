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

		if (m_pipeline = CreateGraphicsPipeline(); !pipeline)
			return;
	}

	void VulkanWindowContext::Resized(int width, int height) {
		DestroySwapchain();
		CreateSwapchain(width, height);
	}

	void VulkanWindowContext::Shutdown() {
		m_shader->Unload();
		m_shader.Reset();

		if (pipelineLayout != VK_NULL_HANDLE) {
			vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
		}

		DestroySwapchain();

		if (surface != VK_NULL_HANDLE) {
			andromeda::trace("Cleaned up surface");
			SDL_Vulkan_DestroySurface(vulkan->GetInstance(), surface, nullptr);
			surface = nullptr;
		}
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

	VkPipeline VulkanWindowContext::CreateGraphicsPipeline() {
		VkPipelineLayoutCreateInfo pipelineLayoutInfo{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
			.setLayoutCount = 0,
			.pushConstantRangeCount = 0,
		};

		VK_CHECK_ELSE_RETURN(vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout), nullptr);

		auto shader = static_cast<VulkanShader*>(m_shader.AsPtr());
		auto mods = shader->GetShaderModules();

		std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
		shaderStages.resize(mods.size());
		for (int i = 0; i < mods.size(); i++) {
			auto& mod = mods[i];
			shaderStages[i] = mod.GetShaderStage();
		}

		// vertex pulling, don't define vertex input details
		VkPipelineVertexInputStateCreateInfo vertInputInfo{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
		};

		// input assembly, we'll be drawing triangle lists
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
			.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
		};

		// depth/stencil configuration
		VkPipelineDepthStencilStateCreateInfo depthStencilInfo{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
			.depthTestEnable = VK_TRUE,
			.depthWriteEnable = VK_TRUE,
			.depthCompareOp = VK_COMPARE_OP_LESS,
			.stencilTestEnable = VK_FALSE,
		};

		// dynamic rendering allows to set this up.. dynamically
		// wel still need this struct though
		VkPipelineViewportStateCreateInfo viewportInfo{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
			.viewportCount = 1,
			.pViewports = nullptr,
			.scissorCount = 1,
			.pScissors = nullptr,
		};

		return nullptr;
	}

	void VulkanWindowContext::SetupIMGUI(ImGui_ImplVulkanH_Window* wd) {
		// wd->Surface = surface;
		// wd->Swapchain = swapchain;
		// wd->Width = swapchainWidth;
		// wd->Height = swapchainHeight;
	}
} // namespace andromeda::graphics