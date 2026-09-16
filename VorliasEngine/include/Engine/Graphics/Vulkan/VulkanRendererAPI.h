#pragma once
#include "../RendererAPI.h"
#include "VulkanInstance.h"
#include "Engine/Common.h"

namespace andromeda::graphics {
	class VulkanRenderer : public Renderer {
	public:
		[[nodiscard]] bool Initialize() override;
		void Clear() override;
		void SetClearColor(Color color) override;
		void Shutdown() override;

		[[nodiscard]] const std::string GetAPIString() const override;
		inline Renderer::API GetAPI() override {
			return Renderer::API::Vulkan;
		}

		GraphicsContext* CreateWindowGraphicsContext(SDL_Window* window) override;
		ScopeRef<GraphicsContext> CreateGraphicsContext(SDL_Window* window);

		VulkanContext* GetContext() {
			return m_context;
		}

		~VulkanRenderer() override;

		// Start a single command buffer
		VkCommandBuffer StartTransientCommandBuffer() const;
		void SubmitTransientCommandBuffer(VkCommandBuffer buffer) const;

		std::vector<uint32_t> UploadImages(const std::vector<Image>& images) override;

		// Create an image on the GPU
		std::pair<uint32_t, GPUBuffer> CreateImage(
			VkCommandBuffer commandBuffer,
			unsigned char* imageData,
			uint32_t width,
			uint32_t height,
			int channels
		);

		uint32_t CreateSampler();
	private:
		void MapCopyBufferData(const GPUBuffer& buffer, size_t bufferOffset, void* data, size_t byteSize);
		GPUBuffer CreateBuffer(VkBufferUsageFlags usage, size_t byteSize, bool mappable, VmaMemoryUsage memoryUsage);
	private:
		VulkanContext* m_context{nullptr};

		// eventually want to deprecate/change this
		std::vector<GraphicsContext> m_contexts{};


		// single use command buffers
		VkCommandPool m_commandPool = VK_NULL_HANDLE;

		// images and buffers
		std::vector<GPUImage> m_images{};
		std::vector<GPUBuffer> m_buffers{};
		std::vector<GPUTexture> m_textures{};
		std::vector<VkSampler> m_samplers{};
		uint32_t m_whiteImagePixelId;
	};
} // namespace andromeda::graphics