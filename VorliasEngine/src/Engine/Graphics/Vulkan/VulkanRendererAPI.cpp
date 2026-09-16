#include "Engine/Graphics/Vulkan/VulkanRendererAPI.h"
#include "Engine/Graphics/Vulkan/VulkanWindowContext.h"
#include "Engine/Log.h"

namespace andromeda::graphics {
	bool VulkanRenderer::Initialize() {
		if (m_context != nullptr) {
			return false;
		}

		m_context = new VulkanContext();
		if (!m_context->InitVulkan())
			return false;

		VkCommandPoolCreateInfo poolInfo{
			.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
			.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT,
			.queueFamilyIndex = static_cast<uint32_t>(m_context->GetGraphicsFamilyIndex()),
		};
		if (vkCreateCommandPool(m_context->GetDevice(), &poolInfo, nullptr, &m_commandPool) != VK_SUCCESS) {
			andromeda::error("Failed to create command buffer pool");
			return false;
		}

		// Create our default fallback image texture
		{
			auto whitePixelImage = Image::WHITE_PIXEL;
			VkCommandBuffer whiteImageCmdBuff = StartTransientCommandBuffer();

			auto [whiteImageId, whiteStagingBuffer] =
				CreateImage(whiteImageCmdBuff, whitePixelImage.data, whitePixelImage.size.x, whitePixelImage.size.y, whitePixelImage.channels);
			m_whiteImagePixelId = whiteImageId;

			SubmitTransientCommandBuffer(whiteImageCmdBuff);
			whiteStagingBuffer.Destroy(m_context->GetAllocator());

			uint32_t whiteSamplerId = CreateSampler();

			m_textures.push_back(
				GPUTexture{
					.imageId = whiteImageId,
					.samplerId = whiteSamplerId,
				}
			);
		}

		andromeda::trace("Vulkan Initialized");
		return true;
	}

	void VulkanRenderer::Clear() {}

	void VulkanRenderer::SetClearColor(Color color) {}

	VkCommandBuffer VulkanRenderer::StartTransientCommandBuffer() const {
		VkCommandBufferAllocateInfo cmdAllocateInfo{
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
			.commandPool = m_commandPool,
			.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
			.commandBufferCount = 1,
		};

		VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
		if (vkAllocateCommandBuffers(m_context->GetDevice(), &cmdAllocateInfo, &commandBuffer) != VK_SUCCESS) {
			andromeda::error("Unable to allocate command buffer");
			return VK_NULL_HANDLE;
		}

		VkCommandBufferBeginInfo beginInfo{
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
			.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
		};

		if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
			andromeda::error("unable to begin command buffer");
			vkFreeCommandBuffers(m_context->GetDevice(), m_commandPool, 1, &commandBuffer);
			return VK_NULL_HANDLE;
		}

		return commandBuffer;
	}
	void VulkanRenderer::SubmitTransientCommandBuffer(VkCommandBuffer commandBuffer) const {
		vkEndCommandBuffer(commandBuffer);
		VkSubmitInfo submitInfo{
			.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
			.commandBufferCount = 1,
			.pCommandBuffers = &commandBuffer,
		};

		vkQueueSubmit(m_context->GetGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(m_context->GetGraphicsQueue());
		vkFreeCommandBuffers(m_context->GetDevice(), m_commandPool, 1, &commandBuffer);
	}

	std::vector<uint32_t> VulkanRenderer::UploadImages(const std::vector<Image>& images) {
		VkCommandBuffer commandBuffer = StartTransientCommandBuffer();
		std::vector<GPUBuffer> stagingBuffers;
		stagingBuffers.reserve(images.size());

		std::vector<uint32_t> imageIds(images.size());
		for (int i = 0; i < images.size(); i++) {
			const Image& image = images[i];
			if (image.data) {
				auto [imageId, stagingTextureBuffer] = CreateImage(commandBuffer, image.data, image.size.x, image.size.y, image.channels);
				imageIds[i] = imageId;
				stagingBuffers.push_back(stagingTextureBuffer);
			} else {
				imageIds[i] = m_whiteImagePixelId; // default texture fallback
			}
		}

		SubmitTransientCommandBuffer(commandBuffer); // submit and wait

		// Clean up the staging buffers
		for (GPUBuffer& stagingBuffer : stagingBuffers) {
			vmaDestroyBuffer(m_context->GetAllocator(), stagingBuffer.vkBuffer, stagingBuffer.allocation);
		}

		return imageIds;
	}

	std::pair<uint32_t, GPUBuffer> VulkanRenderer::CreateImage(
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
		if (vmaCreateImage(m_context->GetAllocator(), &imageInfo, &allocInfo, &gpuImage.image, &gpuImage.allocation, nullptr) != VK_SUCCESS) {
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
		if (vkCreateImageView(m_context->GetDevice(), &imgViewInfo, nullptr, &gpuImage.imageView) != VK_SUCCESS) {
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

	uint32_t VulkanRenderer::CreateSampler() {
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
		if (vkCreateSampler(m_context->GetDevice(), &samplerInfo, nullptr, &sampler) != VK_SUCCESS) {
			andromeda::error("Unable to create texture sampler");
			return false;
		}

		m_samplers.push_back(sampler);
		return m_samplers.size();
	}

	void VulkanRenderer::MapCopyBufferData(const GPUBuffer& buffer, size_t bufferOffset, void* data, size_t byteSize) {
		// map and write buffer data
		void* bufferPtr = nullptr;
		if (vmaMapMemory(m_context->GetAllocator(), buffer.allocation, &bufferPtr) != VK_SUCCESS) {
			andromeda::error("Unable to map buffer memory");
			return;
		}

		std::memcpy(static_cast<char*>(bufferPtr) + bufferOffset, data, byteSize);
		vmaUnmapMemory(m_context->GetAllocator(), buffer.allocation);
	}

	GPUBuffer VulkanRenderer::CreateBuffer(VkBufferUsageFlags usage, size_t byteSize, bool mappable, VmaMemoryUsage memoryUsage) {
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
		if (vmaCreateBuffer(m_context->GetAllocator(), &buffInfo, &allocInfo, &gpuBuffer.vkBuffer, &gpuBuffer.allocation, nullptr) != VK_SUCCESS) {
			return GPUBuffer{};
		}

		if (usage & VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT) {
			VkBufferDeviceAddressInfo vertBdaInfo{
				.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO,
				.buffer = gpuBuffer.vkBuffer,
			};

			gpuBuffer.deviceAddress = vkGetBufferDeviceAddress(m_context->GetDevice(), &vertBdaInfo);
		}

		return gpuBuffer;
	}

	void VulkanRenderer::Shutdown() {
		// wait in case resources are in use
		vkDeviceWaitIdle(m_context->GetDevice());

		for (auto& sampler : m_samplers) {
			vkDestroySampler(m_context->GetDevice(), sampler, nullptr);
		}

		for (auto& image : m_images) {
			vmaDestroyImage(m_context->GetAllocator(), image.image, image.allocation);
			vkDestroyImageView(m_context->GetDevice(), image.imageView, nullptr);
		}

		if (m_commandPool != VK_NULL_HANDLE) {
			vkDestroyCommandPool(m_context->GetDevice(), m_commandPool, nullptr);
		}

		m_context->Shutdown();
		andromeda::trace("Shutdown Vulkan Renderer");
		delete m_context;
	}
} // namespace andromeda::graphics

const std::string andromeda::graphics::VulkanRenderer::GetAPIString() const {
	return std::string("Vulkan") + " " + std::to_string(VK_VERSION_MAJOR(VulkanContext::VulkanVersion)) + "." +
	       std::to_string(VK_VERSION_MINOR(VulkanContext::VulkanVersion));
}

andromeda::graphics::GraphicsContext* andromeda::graphics::VulkanRenderer::CreateWindowGraphicsContext(SDL_Window* window) {
	andromeda::trace("Create new Vulkan Window Context");
	return new VulkanWindowContext(m_context, window);
}


andromeda::ScopeRef<andromeda::graphics::GraphicsContext> andromeda::graphics::VulkanRenderer::CreateGraphicsContext(SDL_Window* window) {
	return CreateScopeRef<andromeda::graphics::VulkanWindowContext>(m_context, window);
}

andromeda::graphics::VulkanRenderer::~VulkanRenderer() {}