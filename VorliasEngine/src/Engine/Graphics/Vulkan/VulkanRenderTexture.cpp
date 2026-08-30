#include "Engine/Graphics/Vulkan/VulkanRenderTexture.h"

using namespace andromeda::graphics;

bool VulkanRenderTexture::Create(VulkanContext* vk, VulkanWindowContext* vkw, int width, int height, VkFormat format) {
	m_width = width;
	m_height = height;
	m_format = format;

	m_ctx = vk;
	m_wctx = vkw;

	if (!CreateImage() || !CreateImageView() || !CreateSampler()) {
		Destroy();
		return false;
	}

	if ((m_textureType & RENDER_TEXTURE_IMGUI) != 0) {
		m_imguiDescriptor = ImGui_ImplVulkan_AddTexture(m_sampler, m_imageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	}

	m_isValid = true;
	m_currentLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	return true;
}

bool VulkanRenderTexture::CreateImage() {
	VkImageCreateInfo imageInfo{
		.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
		.imageType = VK_IMAGE_TYPE_2D,
		.format = m_format,
		.extent = VkExtent3D{m_width, m_height, 1},
		.mipLevels = 1,
		.arrayLayers = 1,
		.samples = VK_SAMPLE_COUNT_1_BIT,
		.tiling = VK_IMAGE_TILING_OPTIMAL,
		.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
		.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
		.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED
	};

	VmaAllocationCreateInfo allocationCreateInfo{};
	allocationCreateInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;

	VkResult result = vmaCreateImage(m_ctx->GetAllocator(), &imageInfo, &allocationCreateInfo, &m_image, &m_allocation, nullptr);
	if (result != VK_SUCCESS) {
		andromeda::error("Failed to create image");
		return false;
	}

	return true;
}

bool VulkanRenderTexture::CreateImageView() {
	VkImageViewCreateInfo viewInfo{
		.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
		.image = m_image,
		.viewType = VK_IMAGE_VIEW_TYPE_2D,
		.format = m_format,
		.subresourceRange = {.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT, .baseMipLevel = 0, .levelCount = 1, .baseArrayLayer = 0, .layerCount = 1}
	};

	if (vkCreateImageView(m_ctx->GetDevice(), &viewInfo, nullptr, &m_imageView) != VK_SUCCESS) {
		return false;
	}

	return true;
}

bool VulkanRenderTexture::CreateSampler() {
	VkSamplerCreateInfo samplerInfo{
		.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
		.magFilter = VK_FILTER_LINEAR,
		.minFilter = VK_FILTER_LINEAR,
		.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
		.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
		.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
		.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
		.mipLodBias = 0.0f,
		.anisotropyEnable = VK_FALSE,
		.compareEnable = VK_FALSE,
		.minLod = 0.0f,
		.maxLod = 0.0f,
		.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK,
		.unnormalizedCoordinates = VK_FALSE
	};

	if (vkCreateSampler(m_ctx->GetDevice(), &samplerInfo, nullptr, &m_sampler) != VK_SUCCESS) {
		return false;
	}

	return true;
}

void VulkanRenderTexture::TransitionImage(VkCommandBuffer commandBuffer, VkImageLayout oldLayout, VkImageLayout newLayout) {
	VkImageMemoryBarrier barrier{
		.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
		.oldLayout = oldLayout,
		.newLayout = newLayout,
		.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.image = m_image,
		.subresourceRange = {.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT, .baseMipLevel = 0, .levelCount = 1, .baseArrayLayer = 0, .layerCount = 1}
	};

	VkPipelineStageFlags sourceStage;
	VkPipelineStageFlags destinationStage;

	if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	} else if (oldLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
		barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
		barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		sourceStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		destinationStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	} else if (oldLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
		barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		sourceStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	} else {
		throw std::runtime_error("Unsupported RenderTexture layout transition");
	}

	vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);
}

void VulkanRenderTexture::Resize(int width, int height) {
	if (width == 0 || height == 0) {
		return;
	}

	if (width == m_width && height == m_height) {
		return;
	}

	RenderTarget::Resize(width, height);
	Destroy();
	Create(m_ctx, m_wctx, width, height, m_format);
}

void VulkanRenderTexture::TransitionToColorAttachment(VkCommandBuffer commandBuffer) {
	if (m_currentLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
		return;
	}

	TransitionImage(commandBuffer, m_currentLayout, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

	m_currentLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
}

void VulkanRenderTexture::TransitionToShaderRead(VkCommandBuffer commandBuffer) {
	if (m_currentLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
		return;
	}

	TransitionImage(commandBuffer, m_currentLayout, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	m_currentLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
}

void VulkanRenderTexture::BeginRender(const FrameResources& res) {
	// 4. Transition image layout (in command buffer)
	m_imageMemoryBarrier = {
		.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
		.image = m_image,
		.subresourceRange =
			{
				.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
				.baseMipLevel = 0,
				.levelCount = 1,
				.baseArrayLayer = 0,
				.layerCount = 1,
			},
		.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
		.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		.srcAccessMask = 0,
		.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
	};

	vkCmdPipelineBarrier(
		res.commandBuffer,
		VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		0,
		0,
		NULL,
		0,
		NULL,
		1,
		&m_imageMemoryBarrier
	);

	// 5. Begin dynamic rendering
	VkRenderingAttachmentInfoKHR colorAttachment = {
		.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR,
		.imageView = m_imageView,
		.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
		.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
		.clearValue = {{0.0f, 0.0f, 0.0f, 1.0f}},
	};

	VkRenderingInfoKHR renderingInfo = {
		.sType = VK_STRUCTURE_TYPE_RENDERING_INFO_KHR,
		.renderArea = {{0, 0}, {m_width, m_height}},
		.layerCount = 1,
		.colorAttachmentCount = 1,
		.pColorAttachments = &colorAttachment,
	};

	vkCmdBeginRendering(res.commandBuffer, &renderingInfo);
}

void VulkanRenderTexture::EndRender(const FrameResources& res) {
	// 7. End rendering
	vkCmdEndRendering(res.commandBuffer);

	// 8. Transition back to readable layout if needed
	m_imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	m_imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	m_imageMemoryBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	m_imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
	vkCmdPipelineBarrier(
		res.commandBuffer,
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
		0,
		0,
		NULL,
		0,
		NULL,
		1,
		&m_imageMemoryBarrier
	);
}

void VulkanRenderTexture::Destroy() {
	if (m_ctx == nullptr) {
		RenderTarget::Destroy();
		return;
	}

	vkDeviceWaitIdle(m_ctx->GetDevice());
	if ((m_textureType & RENDER_TEXTURE_IMGUI) != 0 && m_imguiDescriptor != VK_NULL_HANDLE) {
		ImGui_ImplVulkan_RemoveTexture(m_imguiDescriptor);
		m_imguiDescriptor = VK_NULL_HANDLE;
	}

	if (m_sampler != nullptr) {
		vkDestroySampler(m_ctx->GetDevice(), m_sampler, nullptr);
		m_sampler = VK_NULL_HANDLE;
	}

	if (m_imageView != nullptr) {
		vkDestroyImageView(m_ctx->GetDevice(), m_imageView, nullptr);
		m_imageView = VK_NULL_HANDLE;
	}

	if (m_image != nullptr) {
		vmaDestroyImage(m_ctx->GetAllocator(), m_image, m_allocation);
		m_image = VK_NULL_HANDLE;
		m_allocation = VK_NULL_HANDLE;
	}

	m_isValid = false;
	RenderTarget::Destroy();
}

VulkanRenderTexture::~VulkanRenderTexture() {}