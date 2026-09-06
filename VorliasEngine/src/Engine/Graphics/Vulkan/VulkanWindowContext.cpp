#include "Engine/Graphics/Vulkan/VulkanWindowContext.h"
#include "Engine/Graphics/Vulkan/VulkanUtils.h"
#include "Engine/Log.h"
#include "Engine/Graphics/Vulkan/VulkanRenderTexture.h"


#include "Engine/Graphics/Vulkan/VulkanBase.h"

#include <SDL3/SDL_vulkan.h>
#include <spdlog/spdlog.h>
#include "Engine/Graphics/Vulkan/VulkanShader.h"

andromeda::graphics::PipelineId andromeda::graphics::VulkanWindowContext::s_pipelineIdx = 0;

void andromeda::graphics::VulkanWindowContext::SetVulkanRenderTexture(andromeda::graphics::VulkanRenderTexture* rt) {
	m_renderTexture = rt;
}

void andromeda::graphics::VulkanWindowContext::SetRenderTarget(std::shared_ptr<RenderTexture> renderTarget) {
	SetVulkanRenderTexture(static_cast<VulkanRenderTexture*>(renderTarget.get()));
}

void andromeda::graphics::VulkanWindowContext::RenderToTarget(VulkanRenderTexture* renderTexture) {
	auto* target = renderTexture != nullptr ? renderTexture : m_renderTexture;
	if (target == nullptr || !target->IsValid()) {
		return;
	}

	FrameResources& res = m_frameResources[frameResIdx];
	target->BeginRender(res);
	{
		VkViewport viewport{.x = 0, .y = 0, .width = static_cast<float>(target->GetWidth()), .height = static_cast<float>(target->GetHeight())};
		vkCmdSetViewport(res.commandBuffer, 0, 1, &viewport);

		VkRect2D scissor{
			.offset{.x = 0, .y = 0}, .extent{.width = static_cast<uint32_t>(target->GetWidth()), .height = static_cast<uint32_t>(target->GetHeight())}
		};
		vkCmdSetScissor(res.commandBuffer, 0, 1, &scissor);

		for (auto& command : m_renderCommands) {
			command->Bind(this);
			command->Draw(this);
		}
	}
	target->EndRender(res);
}

namespace andromeda::graphics {
	VulkanWindowContext::VulkanWindowContext(VulkanContext* vulkan, SDL_Window* window) : vulkan(vulkan), window(window) {
		andromeda::trace("Create window context");
	}

	API VulkanWindowContext::GetAPI() {
		return API::Vulkan;
	}

	void VulkanWindowContext::DrawDemoTriangle() {
		FrameResources& res = m_frameResources[frameResIdx];

		vkCmdBindPipeline(res.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_graphicsPipeline->GetPipeline());
		vkCmdDraw(res.commandBuffer, 3, 1, 0, 0);
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

		// m_graphicsPipeline = new VulkanGraphicsPipeline(vulkan, swapchainFormat, depthFormat, static_cast<VulkanShader*>(m_shader.AsPtr()));
		// if (!m_graphicsPipeline->Create()) {
		// 	return;
		// }

		m_graphicsPipeline = CreatePipeline(static_cast<VulkanShader*>(m_shader.AsPtr()));
		if (!m_graphicsPipeline) {
			return;
		}

		if (!CreateSyncResources()) {
			return;
		}

		if (!CreateCommandBuffers()) {
			andromeda::error("Could not create command buffers");
			return;
		}

		// Create our default fallback image texture
		{
			auto whitePixelImage = Image::WHITE_PIXEL;
			VkCommandBuffer whiteImageCmdBuff = StartTransientCommandBuffer();

			auto [whiteImageId, whiteStagingBuffer] =
				CreateImage(whiteImageCmdBuff, whitePixelImage.data, whitePixelImage.size.x, whitePixelImage.size.y, whitePixelImage.channels);
			m_whiteImagePixelId = whiteImageId;

			SubmitTransientCommandBuffer(whiteImageCmdBuff);
			vmaDestroyBuffer(vulkan->GetAllocator(), whiteStagingBuffer.vkBuffer, whiteStagingBuffer.allocation);

			VkSamplerCreateInfo samplerInfo{
				.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
				.magFilter = VK_FILTER_NEAREST,
				.minFilter = VK_FILTER_NEAREST,
				.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT,
				.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT,
				.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT,
				.compareEnable = VK_FALSE,
			};
			VkSampler sampler = VK_NULL_HANDLE;
			if (vkCreateSampler(vulkan->GetDevice(), &samplerInfo, nullptr, &sampler) != VK_SUCCESS) {
				andromeda::error("Unable to create texture sampler");
				return;
			}

			m_samplers.push_back(sampler);
			uint32_t whiteSamplerId = m_samplers.size();

			m_textures.push_back(
				GPUTexture{
					.imageId = whiteImageId,
					.samplerId = whiteSamplerId,
				}
			);
		}
	}

	void VulkanWindowContext::Resize(int width, int height) {
		this->width = width;
		this->height = height;
		m_swapchainRequiresRecreate = true;
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
		vkDeviceWaitIdle(device);

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
			swapchain = VK_NULL_HANDLE;
		}

		if (depthImageView) {
			vkDestroyImageView(device, depthImageView, nullptr);
			vmaDestroyImage(vulkan->GetAllocator(), depthImage, depthImageAllocation);
			depthImageView = VK_NULL_HANDLE;
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
		VkCommandPoolCreateInfo poolInfo{
			.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
			.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT,
			.queueFamilyIndex = static_cast<uint32_t>(vulkan->GetGraphicsFamilyIndex()),
		};
		if (vkCreateCommandPool(vulkan->GetDevice(), &poolInfo, nullptr, &m_commandPool) != VK_SUCCESS) {
			andromeda::error("Failed to create command buffer pool");
			return false;
		}

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

	void VulkanWindowContext::BeforeRender() {
		m_state = VULKAN_STATE_BEFORE_RENDER;

		for (auto& command : m_renderCommands) {
			command->BeforeRender(this);
		}

		// First check if swapchain is valid, if not we'll recreate it
		if (m_swapchainRequiresRecreate) {
			vkDeviceWaitIdle(vulkan->GetDevice());
			DestroySwapchain();
			CreateSwapchain(width, height);
			m_swapchainRequiresRecreate = false;
		}

		frameResIdx = frameIndex++ % MaxFramesInFlight; // get current frame resource idx
		signalValue = nextSignalValue++; // value that current frame will set timeline semaphore to when completed, this will be the
		                                 // value it will wait on for resource
		const uint64_t waitValue = signalValue - MaxFramesInFlight; // value that the current frame will wait on, before begins using the resources

		VkSemaphoreWaitInfo waitInfo{
			.sType = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO,
			.semaphoreCount = 1,
			.pSemaphores = &m_timelineSemaphore,
			.pValues = &waitValue,
		};
		vkWaitSemaphores(vulkan->GetDevice(), &waitInfo, UINT64_MAX);

		// now safe to start recording commands
		FrameResources& res = m_frameResources[frameResIdx];
		vkResetCommandPool(vulkan->GetDevice(), res.commandPool, 0);

		// Begin recording commands
		VkCommandBufferBeginInfo cmdBeginInfo{
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
			.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
		};

		vkBeginCommandBuffer(res.commandBuffer, &cmdBeginInfo);
		m_cmdBuffer = true;
	}

	bool VulkanWindowContext::HasRenderTarget() const {
		return m_renderTexture != nullptr && m_renderTexture->IsValid();
	}

	void VulkanWindowContext::SubmitCommand(std::unique_ptr<RenderCommand> command) {
		ANDROMEDA_ASSERT(command != nullptr);
		m_renderCommands.push_back(std::move(command));
	}

	void VulkanWindowContext::RenderPrepare() {
		m_state = VULKAN_STATE_RENDER_PREPARE;
		FrameResources& res = m_frameResources[frameResIdx];

		// get the resources for this frame
		VkSemaphore imageAcquireSemaphore = m_frameResources[frameResIdx].imageAcquiredSemaphore;

		imageIndex = 0;
		VkResult acquireResult =
			vkAcquireNextImageKHR(vulkan->GetDevice(), swapchain, UINT64_MAX, imageAcquireSemaphore, VK_NULL_HANDLE, &imageIndex);

		// Handle resize and out of date images
		if (acquireResult == VK_ERROR_OUT_OF_DATE_KHR) { // can no longer be used, requires recreate IMMEDIATELY.
			m_swapchainRequiresRecreate = true;
			return;
		} else if (acquireResult == VK_SUBOPTIMAL_KHR) {
			m_swapchainRequiresRecreate = true; // we can recreate the next frame
		}

		// transition the colour and depth images
		std::vector<VkImageMemoryBarrier2KHR> layoutBarriers{
			// barriers
			{.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
		     .srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
		     .srcAccessMask = 0,
		     .dstStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
		     .dstAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
		     .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
		     .newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		     .image = swapchainImages[imageIndex],
		     .subresourceRange{
				 .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
				 .baseMipLevel = 0,
				 .levelCount = 1,
				 .baseArrayLayer = 0,
				 .layerCount = 1,
			 }},
			{.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
		     .srcStageMask = VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT,
		     .srcAccessMask = 0,
		     .dstStageMask = VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT |
		                     VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT, // both specified to control memory access at both stages (write)
		     .dstAccessMask = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
		     .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
		     .newLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
		     .image = depthImage,
		     .subresourceRange{
				 .aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT,
				 .baseMipLevel = 0,
				 .levelCount = 1,
				 .baseArrayLayer = 0,
				 .layerCount = 1,
			 }}
		};

		VkDependencyInfo depInfo{
			.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
			.imageMemoryBarrierCount = static_cast<uint32_t>(layoutBarriers.size()),
			.pImageMemoryBarriers = layoutBarriers.data()
		};
		vkCmdPipelineBarrier2(res.commandBuffer, &depInfo);

		// Optional offscreen render target, but the normal window render pass still must continue for ImGui/UI.
		if (m_renderTexture != nullptr && m_renderTexture->IsValid()) {
			RenderToTarget(m_renderTexture);
		}

		// setup the attachments (color and depth) and begin rendering (dynamic)
		VkRenderingAttachmentInfo colorAttachInfo{
			.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
			.imageView = swapchainImageViews[imageIndex],
			.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
			.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
			.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
			.clearValue{m_clearColor}
		};

		VkRenderingAttachmentInfo depthAttachInfo{
			.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
			.imageView = depthImageView,
			.imageLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
			.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
			.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
			.clearValue{.depthStencil{1.0f, 0}}
		};

		VkRenderingInfo renderingInfo{
			.sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
			.renderArea = {.offset = {.x = 0, .y = 0}, .extent = {.width = swapchainWidth, .height = swapchainHeight}},
			.layerCount = 1,
			.colorAttachmentCount = 1,
			.pColorAttachments = &colorAttachInfo,
			.pDepthAttachment = &depthAttachInfo
		};



		vkCmdBeginRendering(res.commandBuffer, &renderingInfo);
	}

	void VulkanWindowContext::RenderDraw() {
		m_state = VULKAN_STATE_RENDER;

		FrameResources& res = m_frameResources[frameResIdx];

		// set the viewport and scissor state
		VkViewport viewport{.x = 0, .y = 0, .width = static_cast<float>(swapchainWidth), .height = static_cast<float>(swapchainHeight)};
		vkCmdSetViewport(res.commandBuffer, 0, 1, &viewport);

		VkRect2D scissor{.offset{.x = 0, .y = 0}, .extent{.width = swapchainWidth, .height = swapchainHeight}};
		vkCmdSetScissor(res.commandBuffer, 0, 1, &scissor);

		// // draw our triangle
		// vkCmdBindPipeline(res.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_graphicsPipeline->GetPipeline());
		// vkCmdDraw(res.commandBuffer, 3, 1, 0, 0);

		if (m_renderTexture == nullptr) {
			for (auto& command : m_renderCommands) {
				command->Bind(this);
				command->Draw(this);
			}
		}
	}

	void VulkanWindowContext::RenderPresent() {
		m_state = VULKAN_STATE_RENDER_PRESENT;

		FrameResources& res = m_frameResources[frameResIdx];
		vkCmdEndRendering(res.commandBuffer);

		// transition the image from color attachment to presentation so we can show it
		VkImageMemoryBarrier2 presentLayoutBarrier{
			.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
			.srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
			.srcAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
			.dstStageMask = VK_PIPELINE_STAGE_2_NONE, // nothing is waiting, but the cache is flushed and layout is transition
			.dstAccessMask = 0,
			.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
			.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
			.image = swapchainImages[imageIndex],
			.subresourceRange{
				.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
				.baseMipLevel = 0,
				.levelCount = 1,
				.baseArrayLayer = 0,
				.layerCount = 1,
			}
		};
		VkDependencyInfo presentDepInfo{
			.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO, .imageMemoryBarrierCount = 1, .pImageMemoryBarriers = &presentLayoutBarrier
		};
		vkCmdPipelineBarrier2(res.commandBuffer, &presentDepInfo);

		vkEndCommandBuffer(res.commandBuffer);
		m_cmdBuffer = false;

		// ensure swapchain image is actually vailable to start color output
		VkSemaphoreSubmitInfo imageAcquireWaitInfo{
			.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
			.semaphore = res.imageAcquiredSemaphore,
			.stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT // wait before drawing to image
		};
		// signal that the image can be presented
		std::vector<VkSemaphoreSubmitInfo> semaphoreSignals{
			{// render work completion signal
		     .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
		     .semaphore = renderCompleteSemaphores[imageIndex],
		     .stageMask = VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT
		    },
			{// entire frame is completed (timeline)
		     .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
		     .semaphore = m_timelineSemaphore,
		     .value = signalValue,
		     .stageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT
		    }
		};
		VkCommandBufferSubmitInfo cmdSubmitInfo{
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO,
			.commandBuffer = res.commandBuffer,
		};
		VkSubmitInfo2 submitInfo{
			.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2,
			.waitSemaphoreInfoCount = 1,
			.pWaitSemaphoreInfos = &imageAcquireWaitInfo, // ensure the image is ready
			.commandBufferInfoCount = 1,
			.pCommandBufferInfos = &cmdSubmitInfo,
			.signalSemaphoreInfoCount = static_cast<uint32_t>(semaphoreSignals.size()),
			.pSignalSemaphoreInfos = semaphoreSignals.data()
		};
		vkQueueSubmit2(vulkan->GetGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);

		// present the image
		VkPresentInfoKHR presentInfo{
			.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
			.waitSemaphoreCount = 1,
			.pWaitSemaphores = &renderCompleteSemaphores[imageIndex], // render work completed semaphore
			.swapchainCount = 1,
			.pSwapchains = &swapchain,
			.pImageIndices = &imageIndex,
			.pResults = nullptr
		};

		vkQueuePresentKHR(vulkan->GetGraphicsQueue(), &presentInfo);

		m_renderCommands.clear();
		m_state = VULKAN_STATE_POST_RENDER;
	}

	VkCommandBuffer VulkanWindowContext::StartTransientCommandBuffer() {
		VkCommandBufferAllocateInfo cmdAllocateInfo{
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
			.commandPool = m_commandPool,
			.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
			.commandBufferCount = 1,
		};

		VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
		if (vkAllocateCommandBuffers(vulkan->GetDevice(), &cmdAllocateInfo, &commandBuffer) != VK_SUCCESS) {
			andromeda::error("Unable to allocate command buffer");
			return VK_NULL_HANDLE;
		}

		VkCommandBufferBeginInfo beginInfo{
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
			.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
		};

		if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
			andromeda::error("unable to begin command buffer");
			vkFreeCommandBuffers(vulkan->GetDevice(), m_commandPool, 1, &commandBuffer);
			return VK_NULL_HANDLE;
		}

		return commandBuffer;
	}

	void VulkanWindowContext::SubmitTransientCommandBuffer(VkCommandBuffer commandBuffer) {
		vkEndCommandBuffer(commandBuffer);
		VkSubmitInfo submitInfo{
			.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
			.commandBufferCount = 1,
			.pCommandBuffers = &commandBuffer,
		};

		vkQueueSubmit(vulkan->GetGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(vulkan->GetGraphicsQueue());
		vkFreeCommandBuffers(vulkan->GetDevice(), m_commandPool, 1, &commandBuffer);
	}

	std::pair<uint32_t, GPUBuffer> VulkanWindowContext::CreateImage(
		VkCommandBuffer commandBuffer,
		unsigned char* imageData,
		uint32_t width,
		uint32_t height,
		int channels
	) {
		VkFormat imageFormat = VK_FORMAT_R8G8B8A8_SRGB;
		VkImageCreateInfo imageInfo{
			.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
			.imageType = VK_IMAGE_TYPE_2D,
			.format = imageFormat,
			.extent{.width = width, .height = height, .depth = 1},
			.mipLevels = 1,
			.arrayLayers = 1,
			.samples = VK_SAMPLE_COUNT_1_BIT,
			.tiling = VK_IMAGE_TILING_OPTIMAL,
			.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
			.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
		};

		VmaAllocationCreateInfo allocInfo{
			.usage = VMA_MEMORY_USAGE_AUTO,
		};

		GPUImage gpuImage;
		if (vmaCreateImage(vulkan->GetAllocator(), &imageInfo, &allocInfo, &gpuImage.image, &gpuImage.allocation, nullptr) != VK_SUCCESS) {
			andromeda::error("Error creating image");
			return {0, GPUBuffer{}};
		}

		VkImageViewCreateInfo imgViewInfo{
			.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
			.image = gpuImage.image,
			.viewType = VK_IMAGE_VIEW_TYPE_2D,
			.format = imageFormat,
			.subresourceRange{
				.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT, // accessing the color of the image
				.levelCount = 1, // 1 mip level and array level
				.layerCount = 1,
			},
		};

		// create a view into the image memory
		if (vkCreateImageView(vulkan->GetDevice(), &imgViewInfo, nullptr, &gpuImage.imageView) != VK_SUCCESS) {
			andromeda::error("Error creating image view");
			return {0, GPUBuffer{}};
		}

		// prepare to upload image data
		VkImageMemoryBarrier2 transferBarrier{
			.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
			.srcStageMask = VK_PIPELINE_STAGE_2_NONE,
			.srcAccessMask = VK_ACCESS_2_NONE,
			.dstStageMask = VK_PIPELINE_STAGE_2_COPY_BIT,
			.dstAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT,
			.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
			.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, // prepares hardware for writing image, ensures caches flushed and visible in VRAM
			.image = gpuImage.image,
			.subresourceRange{
				.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
				.baseMipLevel = 0,
				.levelCount = 1,
				.baseArrayLayer = 0,
				.layerCount = 1,
			},
		};

		VkDependencyInfo transferDepInfo{
			.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
			.imageMemoryBarrierCount = 1,
			.pImageMemoryBarriers = &transferBarrier,
		};
		vkCmdPipelineBarrier2(commandBuffer, &transferDepInfo);

		// create a staging buffer for copying data
		const size_t byteSize = width * height * channels;
		GPUBuffer stageBuffer = CreateBuffer(VK_IMAGE_USAGE_TRANSFER_SRC_BIT, byteSize, true, /* Use CPU */ VMA_MEMORY_USAGE_AUTO_PREFER_HOST);
		MapCopyBufferData(stageBuffer, 0, imageData, byteSize);

		VkBufferImageCopy bufferImageCopy{
			.imageSubresource{
				.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
				.mipLevel = 0,
				.baseArrayLayer = 0,
				.layerCount = 1,
			},
			.imageExtent{.width = width, .height = height, .depth = 1},
		};
		vkCmdCopyBufferToImage(commandBuffer, stageBuffer.vkBuffer, gpuImage.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &bufferImageCopy);

		// transition image for shader read/sampling
		VkImageMemoryBarrier2 shaderReadBarrier{
			.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
			.srcStageMask = VK_PIPELINE_STAGE_2_COPY_BIT,
			.srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT,
			.dstStageMask = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT,
			.dstAccessMask = VK_ACCESS_2_SHADER_READ_BIT,
			.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, // now can be sampled by shaders
			.image = gpuImage.image,
			.subresourceRange{
				.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
				.baseMipLevel = 0,
				.levelCount = 1,
				.baseArrayLayer = 0,
				.layerCount = 1,
			},
		};
		VkDependencyInfo shaderReadDepInfo{
			.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
			.imageMemoryBarrierCount = 1,
			.pImageMemoryBarriers = &shaderReadBarrier,
		};
		vkCmdPipelineBarrier2(commandBuffer, &shaderReadDepInfo);

		m_images.push_back(gpuImage);
		const uint32_t imageId = m_images.size(); // zero will be null
		return {imageId, stageBuffer};
	}

	void VulkanWindowContext::MapCopyBufferData(const GPUBuffer& buffer, size_t bufferOffset, void* data, size_t byteSize) {
		// map and write buffer data
		void* bufferPtr = nullptr;
		if (vmaMapMemory(vulkan->GetAllocator(), buffer.allocation, &bufferPtr) != VK_SUCCESS) {
			andromeda::error("Unable to map buffer memory");
			return;
		}

		std::memcpy(static_cast<char*>(bufferPtr) + bufferOffset, data, byteSize);
		vmaUnmapMemory(vulkan->GetAllocator(), buffer.allocation);
	}

	GPUBuffer VulkanWindowContext::CreateBuffer(VkBufferUsageFlags usage, size_t byteSize, bool mappable, VmaMemoryUsage memoryUsage) {
		// Create buffer and VMA allocation
		VkBufferCreateInfo buffInfo{
			.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
			.size = byteSize,
			.usage = usage,
			.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
		};

		VmaAllocationCreateInfo allocInfo{
			.flags = mappable ? VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT : 0u,
			.usage = memoryUsage,
		};

		GPUBuffer gpuBuffer;
		if (vmaCreateBuffer(vulkan->GetAllocator(), &buffInfo, &allocInfo, &gpuBuffer.vkBuffer, &gpuBuffer.allocation, nullptr) != VK_SUCCESS) {
			return GPUBuffer{};
		}

		if (usage & VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT) {
			VkBufferDeviceAddressInfo vertBdaInfo{
				.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO,
				.buffer = gpuBuffer.vkBuffer,
			};

			gpuBuffer.deviceAddress = vkGetBufferDeviceAddress(vulkan->GetDevice(), &vertBdaInfo);
		}

		return gpuBuffer;
	}

	void VulkanWindowContext::Shutdown() {
		// wait in case resources are in use
		vkDeviceWaitIdle(vulkan->GetDevice());

		for (auto& sampler : m_samplers) {
			vkDestroySampler(vulkan->GetDevice(), sampler, nullptr);
		}

		for (auto& image : m_images) {
			vmaDestroyImage(vulkan->GetAllocator(), image.image, image.allocation);
			vkDestroyImageView(vulkan->GetDevice(), image.imageView, nullptr);
		}

		m_shader->Unload();
		m_shader.Reset();

		if (m_commandPool != VK_NULL_HANDLE) {
			vkDestroyCommandPool(vulkan->GetDevice(), m_commandPool, nullptr);
		}

		// Frame/time cleanup
		{
			for (FrameResources& res : m_frameResources) {
				if (res.imageAcquiredSemaphore != VK_NULL_HANDLE)
					vkDestroySemaphore(vulkan->GetDevice(), res.imageAcquiredSemaphore, nullptr);

				if (res.commandPool != VK_NULL_HANDLE)
					vkDestroyCommandPool(vulkan->GetDevice(), res.commandPool, nullptr); // destroys buffers implicitly
			}

			if (m_timelineSemaphore != VK_NULL_HANDLE) {
				vkDestroySemaphore(vulkan->GetDevice(), m_timelineSemaphore, nullptr);
			}
		}

		// Cleanup the graphics pipeline
		if (m_graphicsPipeline != nullptr) {
			delete m_graphicsPipeline;
		}

		DestroySwapchain();

		if (surface != VK_NULL_HANDLE) {
			andromeda::trace("Cleaned up surface");
			SDL_Vulkan_DestroySurface(vulkan->GetInstance(), surface, nullptr);
			surface = VK_NULL_HANDLE;
		}
	}
} // namespace andromeda::graphics