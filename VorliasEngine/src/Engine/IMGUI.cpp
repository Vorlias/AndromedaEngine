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

		m_context = ctx;

		// We've already set up instance + physicalDevice + queue in ctx
		// Create logical device with one queue
		{
			ImVector<const char*> device_extensions;
			device_extensions.push_back("VK_KHR_swapchain");

			uint32_t properties_count;
			ImVector<VkExtensionProperties> properties;
			vkEnumerateDeviceExtensionProperties(ctx->GetPhysicalDevice(), nullptr, &properties_count, nullptr);
			properties.resize(properties_count);
			vkEnumerateDeviceExtensionProperties(ctx->GetPhysicalDevice(), nullptr, &properties_count, properties.Data);

#ifdef VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME
			if (IsExtensionAvailable(properties, VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME))
				device_extensions.push_back(VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME);
#endif

			const float queue_priority[] = {1.0f};
			VkDeviceQueueCreateInfo queue_info[1] = {{
				.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
				.queueFamilyIndex = (uint32_t)ctx->GetGraphicsFamilyIndex(),
				.queueCount = 1,
				.pQueuePriorities = queue_priority,
			}};

			m_device = ctx->CreateDevice(device_extensions, {queue_info, 1});
			if (m_device == nullptr)
				return;

			vkGetDeviceQueue(m_device, ctx->GetGraphicsFamilyIndex(), 0, &m_queue);
		}

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
				return;
			}
		}
	}

	constexpr static uint32_t g_MinImageCount = 2;
	bool Initialize(andromeda::Window& window) override {
		auto windowContext = static_cast<andromeda::graphics::VulkanWindowContext*>(window.GetGraphicsContext());
		m_surface = windowContext->GetSurface(); // get window surface from window

		andromeda::Vector2i windowSize = window.GetWindowSizeInPixels();

		VkBool32 res = VK_FALSE;
		vkGetPhysicalDeviceSurfaceSupportKHR(m_context->GetPhysicalDevice(), m_context->GetGraphicsFamilyIndex(), m_surface, &res);
		if (res != VK_TRUE) {
			andromeda::error("No WSI support on physical device");
			exit(-1);
		}
		IMGUI_CHECKVERSION();
		auto imguiContext = ImGui::CreateContext();

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
				.RenderPass = VK_NULL_HANDLE,
				.Subpass = 0,
			},
			.UseDynamicRendering = true,
		};

		if (!ImGui_ImplVulkan_Init(&initInfo)) {
			return false;
		}

		if (!ImGui_ImplSDL3_InitForVulkan(window.GetHandle())) {
			return false;
		}

		return true;
	}

	void CleanupContext() override {
		if (m_descriptorPool != VK_NULL_HANDLE) {
			vkDestroyDescriptorPool(m_device, m_descriptorPool, nullptr);
			m_descriptorPool = VK_NULL_HANDLE;
		}

		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplSDL3_Shutdown();

		if (m_device != VK_NULL_HANDLE) {
			vkDestroyDevice(m_device, nullptr);
			m_device = nullptr;
		}
	}

private:
	VkSurfaceKHR m_surface{nullptr};

	andromeda::graphics::VulkanContext* m_context{nullptr};
	VkDevice m_device{VK_NULL_HANDLE};
	VkQueue m_queue{VK_NULL_HANDLE};
	VkDescriptorPool m_descriptorPool{VK_NULL_HANDLE};
	ImGui_ImplVulkanH_Window m_windowData{};
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
