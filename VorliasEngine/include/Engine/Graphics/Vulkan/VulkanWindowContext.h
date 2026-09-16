#pragma once
#include "Engine/Graphics/Vulkan/VulkanBase.h"
#include "Engine/Graphics/GraphicsContext.h"
#include "Engine/Graphics/Vulkan/VulkanShader.h"
#include "Engine/Graphics/Vulkan/VulkanInstance.h"
#include "imgui/imgui_impl_vulkan.h"
#include <SDL3/SDL.h>

#include "Engine/Data/Image.h"
#include "VulkanGraphicsPipeline.h"

namespace andromeda::graphics {
	using PipelineId = uint16_t;

	struct FrameResources {
		VkCommandPool commandPool = VK_NULL_HANDLE;
		VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
		VkSemaphore imageAcquiredSemaphore = VK_NULL_HANDLE;
	};

	enum RenderStateEnum {
		VULKAN_STATE_INIT,
		VULKAN_STATE_BEFORE_RENDER,
		VULKAN_STATE_RENDER_PREPARE,
		VULKAN_STATE_RENDER,
		VULKAN_STATE_RENDER_PRESENT,
		VULKAN_STATE_POST_RENDER,
	};

	class VulkanRenderTexture;

	class VulkanRenderSurface : public RenderSurface {
	public:
		VulkanRenderSurface(VulkanContext* context, SDL_Window* window): m_window(window), m_context(context) {}
	private:
		SDL_Window* m_window;
		VulkanContext* m_context;
		uint32_t m_width, m_height;
	};

	class VulkanWindowContext : public GraphicsContext {
	public:
		constexpr static uint32_t MaxFramesInFlight{2};
		constexpr static VkFormat swapchainFormat{VK_FORMAT_B8G8R8A8_SRGB};
		constexpr static VkFormat depthFormat{VK_FORMAT_D32_SFLOAT}; // represents a depth buffer of 32 bit floats
		static PipelineId s_pipelineIdx;

		RenderStateEnum m_state = VULKAN_STATE_INIT;
		bool m_cmdBuffer = false;

	public:
		VulkanWindowContext(VulkanContext* vulkan, SDL_Window* window);

		void Initialize() override;
		void Shutdown() override;
		void Resize(int width, int height) override;
		API GetAPI() override;

		void SetClearColor(Color color) override {
			m_clearColor = {color.r, color.g, color.b, color.a};
		}

		void DrawDemoTriangle();

		void SubmitCommand(std::unique_ptr<RenderCommand> command) override;

		void BeforeRender() override;
		void RenderPrepare() override;
		void RenderDraw() override;
		void RenderPresent() override;

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

		ANDROMEDA_GETCONST const FrameResources& GetCurrentFrameResources() const {
			return m_frameResources[frameResIdx];
		}

		ANDROMEDA_GETCONST const VkCommandBuffer GetCommandBuffer() const {
			return m_frameResources[frameResIdx].commandBuffer;
		}

		void SetVulkanRenderTexture(VulkanRenderTexture* renderTexture);
		void RenderToTarget(VulkanRenderTexture* renderTexture = nullptr);
		bool HasRenderTarget() const;

		void SetRenderTarget(std::shared_ptr<RenderTexture> renderTarget) override;

		VulkanGraphicsPipeline* GetPipeline(PipelineId pipelineId) { // a bit like a program
			if (!m_pipelines.contains(pipelineId))
				return nullptr;
			return m_pipelines.at(pipelineId);
		}

		VulkanGraphicsPipeline* CreatePipeline(VulkanShader* shader) {
			VulkanGraphicsPipeline* pp = new VulkanGraphicsPipeline(vulkan, swapchainFormat, depthFormat, shader);
			if (!pp->Create())
				return nullptr;

			m_pipelines.insert({s_pipelineIdx, pp});

			std::cout << "create pipeline with id " << s_pipelineIdx << std::endl;
			s_pipelineIdx++;
			return pp;
		}

		// VkCommandBuffer StartTransientCommandBuffer();
		// void SubmitTransientCommandBuffer(VkCommandBuffer commandBuffer);

		// std::pair<uint32_t, GPUBuffer> CreateImage(VkCommandBuffer buffer, unsigned char* imageData, uint32_t width, uint32_t height, int channels);
		// GPUBuffer CreateBuffer(VkBufferUsageFlags usage, size_t byteSize, bool mappable, VmaMemoryUsage memoryUsage);
		// void MapCopyBufferData(const GPUBuffer& buffer, size_t bufferOffset, void* data, size_t byteSize);

		// std::vector<uint32_t> UploadImages(const std::vector<Image>& images) override;
	private:
		[[nodiscard]] bool CreateSurface();
		[[nodiscard]] bool CreateShaders();
		[[nodiscard]] bool CreateSyncResources();
		[[nodiscard]] bool CreateCommandBuffers();

		[[nodiscard]] bool CreateSwapchain(int width, int height);
		void DestroySwapchain();

		VulkanRenderTexture* m_renderTexture{};

		SDL_Window* window;
		VulkanContext* vulkan;
		uint32_t width, height;

		VulkanGraphicsPipeline* m_graphicsPipeline = nullptr;

		// map of shader UUID to pipeline
		std::unordered_map<uint64_t, VulkanGraphicsPipeline*> m_pipelines{};

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

		bool m_swapchainRequiresRecreate = false;
		uint32_t swapchainWidth, swapchainHeight;

		uint64_t frameIndex = 0;
		uint64_t nextSignalValue = MaxFramesInFlight + 1, signalValue = 0;
		uint32_t frameResIdx = 0, imageIndex = 0;

		Ref<Shader> m_shader;

		uint32_t m_minImageCount = 0, m_imageCount = 0;

		// frame and synchronization resources
		VkSemaphore m_timelineSemaphore = VK_NULL_HANDLE;
		std::array<FrameResources, MaxFramesInFlight> m_frameResources;

		// images and buffers
		std::vector<GPUImage> m_images{};
		std::vector<GPUBuffer> m_buffers{};
		std::vector<GPUTexture> m_textures{};
		std::vector<VkSampler> m_samplers{};
		uint32_t m_whiteImagePixelId;

		// single use command buffers
		VkCommandPool m_commandPool = VK_NULL_HANDLE;

		VkClearColorValue m_clearColor{0.01f, 0.01f, 0.01f, 1};
		std::vector<std::unique_ptr<RenderCommand>> m_renderCommands;
	};
} // namespace andromeda::graphics