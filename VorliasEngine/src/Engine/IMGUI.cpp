#include "Engine/IMGUI.h"
#include "Engine/Engine.h"
#include "Engine/Graphics/Vulkan/VulkanInstance.h"
#include "Engine/Graphics/Vulkan/VulkanWindowContext.h"
#include "Engine/Graphics/Vulkan/VulkanRendererAPI.h"
#include "Engine/Log.h"
#include "Engine/Graphics/Vulkan/VulkanUtils.h"


#include "imgui/imgui_impl_vulkan.h"
#include <SDL3/SDL_vulkan.h>
#include "imgui/imgui_impl_sdl3.h"

static bool IsExtensionAvailable(const ImVector<VkExtensionProperties>& properties, const char* extension) {
	for (const VkExtensionProperties& p : properties)
		if (strcmp(p.extensionName, extension) == 0)
			return true;
	return false;
}
class VulkanImWindowContext : public andromeda::ImWindowContext {
public:
	void SetupVulkan(andromeda::graphics::VulkanContext* ctx) {
		if (ctx == nullptr) {
			andromeda::error("Context is nullptr?");
			abort();
		}

		m_context = ctx; // set this
	}

	constexpr static uint32_t g_MinImageCount = 2;
	bool Initialize(andromeda::Window& window) override {
		auto vulkanWindowContext = static_cast<andromeda::graphics::VulkanWindowContext*>(window.GetGraphicsContext());
		m_windowContext = vulkanWindowContext;

		m_surface = vulkanWindowContext->GetSurface(); // get window surface from window
		m_device = vulkanWindowContext->GetDevice();
		m_queue = vulkanWindowContext->GetGraphicsQueue();

		ANDROMEDA_ASSERT((m_context != 0));
		const VkFormat requestSurfaceImageFormat[] = {
			VK_FORMAT_B8G8R8A8_UNORM, VK_FORMAT_R8G8B8A8_UNORM, VK_FORMAT_B8G8R8_UNORM, VK_FORMAT_R8G8B8_UNORM
		};
		const VkColorSpaceKHR requestSurfaceColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
		m_surfaceFormat = ImGui_ImplVulkanH_SelectSurfaceFormat(
			m_context->GetPhysicalDevice(),
			m_surface,
			requestSurfaceImageFormat,
			(size_t)IM_COUNTOF(requestSurfaceImageFormat),
			requestSurfaceColorSpace
		);

		VkPresentModeKHR present_modes[] = {VK_PRESENT_MODE_FIFO_KHR};
		m_presentMode = ImGui_ImplVulkanH_SelectPresentMode(m_context->GetPhysicalDevice(), m_surface, &present_modes[0], IM_COUNTOF(present_modes));

		VkBool32 res = VK_FALSE;
		vkGetPhysicalDeviceSurfaceSupportKHR(m_context->GetPhysicalDevice(), m_context->GetGraphicsFamilyIndex(), m_surface, &res);
		if (res != VK_TRUE) {
			andromeda::error("No WSI support on physical device");
			exit(-1);
		}
		IMGUI_CHECKVERSION();
		auto imguiContext = ImGui::CreateContext();

		// Create Descriptor Pool
		// If you wish to load e.g. additional textures you may need to alter pools sizes and maxSets.
		{
			VkDescriptorPoolSize pool_sizes[] = {
				{VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, IMGUI_IMPL_VULKAN_MINIMUM_SAMPLED_IMAGE_POOL_SIZE},
				{VK_DESCRIPTOR_TYPE_SAMPLER, IMGUI_IMPL_VULKAN_MINIMUM_SAMPLER_POOL_SIZE},
			};
			VkDescriptorPoolCreateInfo pool_info = {};
			pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
			pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
			pool_info.maxSets = 0;
			for (VkDescriptorPoolSize& pool_size : pool_sizes)
				pool_info.maxSets += pool_size.descriptorCount;
			pool_info.poolSizeCount = (uint32_t)IM_COUNTOF(pool_sizes);
			pool_info.pPoolSizes = pool_sizes;
			if (vkCreateDescriptorPool(m_device, &pool_info, nullptr, &m_descriptorPool) != VK_SUCCESS) {
				andromeda::error("Failed to create descriptor pool for IMGUI");
				return false;
			}
		}

		ImGui_ImplVulkan_InitInfo initInfo{
			.ApiVersion = andromeda::graphics::VulkanContext::VulkanVersion,
			.Instance = m_context->GetInstance(),
			.PhysicalDevice = m_context->GetPhysicalDevice(),
			.QueueFamily = (uint32_t)m_context->GetGraphicsFamilyIndex(),
			.Device = m_device,
			.Queue = m_queue,
			.DescriptorPool = m_descriptorPool,
			.ImageCount = 2,
			.MinImageCount = 2,
			.PipelineCache = VK_NULL_HANDLE,
			.PipelineInfoMain{
				.RenderPass = VK_NULL_HANDLE, // dynamic rendering enabled
				.Subpass = 0,
				.PipelineRenderingCreateInfo{
					VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
					.colorAttachmentCount = 1,
					.pColorAttachmentFormats = &andromeda::graphics::VulkanWindowContext::swapchainFormat,
				},
			},
			.UseDynamicRendering = true,
		};

		ImGui_ImplVulkan_LoadFunctions(
			andromeda::graphics::VulkanContext::VulkanVersion,
			[](const char* function_name, void* user_data) {
				return vkGetDeviceProcAddr((VkDevice)user_data, function_name);
			},
			m_device
		);

		if (!ImGui_ImplVulkan_Init(&initInfo)) {
			return false;
		}

		if (!ImGui_ImplSDL3_InitForVulkan(window.GetHandle())) {
			return false;
		}

		return true;
	}

	void NewFrame() override {
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplSDL3_NewFrame();
		ImGui::NewFrame();
	}

	void Render() override {
		ImGui::Render();
		ImDrawData* drawData = ImGui::GetDrawData();

		VkResult err = vkAcquireNextImageKHR( // going to need a separate swapchain here prob. ? 
			/* device */ m_windowContext->GetDevice(),  /*swapchain*/ m_windowContext->GetSwapchain(), UINT64_MAX, /*semaphore*/ nullptr, /*fence*/ nullptr, /*pImageIndex*/ nullptr
		);
	}

	void CleanupContext() override {
		if (m_descriptorPool != VK_NULL_HANDLE) {
			vkDestroyDescriptorPool(m_device, m_descriptorPool, nullptr);
			m_descriptorPool = VK_NULL_HANDLE;
		}

		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplSDL3_Shutdown();
		ImGui::DestroyContext();

		// if (m_device != VK_NULL_HANDLE) {
		// 	vkDestroyDevice(m_device, nullptr);
		// 	m_device = nullptr;
		// }
	}

private:
	VkSurfaceKHR m_surface{nullptr};
	VkSurfaceFormatKHR m_surfaceFormat{};
	VkPresentModeKHR m_presentMode{};

	ImGui_ImplVulkanH_Window* m_imguiWindow{nullptr};

	andromeda::graphics::VulkanContext* m_context{nullptr};
	andromeda::graphics::VulkanWindowContext* m_windowContext{nullptr};

	VkDevice m_device{VK_NULL_HANDLE};
	VkQueue m_queue{VK_NULL_HANDLE};
	VkDescriptorPool m_descriptorPool{VK_NULL_HANDLE};
};

namespace andromeda {
	SharedRef<ImWindowContext> ImWindowContext::Create(graphics::Renderer* renderer) {
		switch (renderer->GetAPI()) {
			case graphics::Renderer::API::Vulkan: {
				auto vulkanRenderer = static_cast<graphics::VulkanRenderer*>(renderer);

				auto handle = CreateScopeRef<VulkanImWindowContext>();
				handle->m_renderer = vulkanRenderer;

				handle->SetupVulkan(vulkanRenderer->GetContext());
				return handle;
			}
		}

		return nullptr;
	}

	void ImWindowContext::Shutdown() {
		CleanupContext();
		m_cleanup = true;
	}
} // namespace andromeda
