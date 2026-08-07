#include "Engine/IMGUI.h"
#include "Engine/Engine.h"
#include "Engine/Graphics/Vulkan/VulkanInstance.h"
#include "Engine/Graphics/Vulkan/VulkanWindowContext.h"
#include "Engine/Graphics/Vulkan/VulkanRendererAPI.h"
#include "Engine/Log.h"


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

			VkDeviceCreateInfo createInfo{
				.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
				.queueCreateInfoCount = sizeof(queue_info) / sizeof(queue_info[0]),
				.pQueueCreateInfos = queue_info,
				.enabledExtensionCount = (uint32_t)device_extensions.Size,
				.ppEnabledExtensionNames = device_extensions.Data,
			};

			if (vkCreateDevice(ctx->GetPhysicalDevice(), &createInfo, nullptr, &m_device) != VK_SUCCESS) {
				andromeda::error("Failed to create device for IMGUI");
				return;
			}
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

		// Check for WSI support
		ImGui_ImplVulkanH_Window* wd = &m_windowData;
		VkBool32 res;

		vkGetPhysicalDeviceSurfaceSupportKHR(m_context->GetPhysicalDevice(), m_context->GetGraphicsFamilyIndex(), m_surface, &res);
		if (res != VK_TRUE) {
			andromeda::error("No WSI support on physical device");
			exit(-1);
		}

		IMGUI_CHECKVERSION();

		ImGui::CreateContext();

		const VkFormat requestSurfaceImageFormat[] = {
			VK_FORMAT_B8G8R8A8_UNORM, VK_FORMAT_R8G8B8A8_UNORM, VK_FORMAT_B8G8R8_UNORM, VK_FORMAT_R8G8B8_UNORM
		};
		const VkColorSpaceKHR requestSurfaceColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
		wd->Surface = m_surface;
		wd->SurfaceFormat = ImGui_ImplVulkanH_SelectSurfaceFormat(
			m_context->GetPhysicalDevice(),
			wd->Surface,
			requestSurfaceImageFormat,
			(size_t)IM_COUNTOF(requestSurfaceImageFormat),
			requestSurfaceColorSpace
		);

		VkPresentModeKHR present_modes[] = {VK_PRESENT_MODE_FIFO_KHR};
		wd->PresentMode =
			ImGui_ImplVulkanH_SelectPresentMode(m_context->GetPhysicalDevice(), wd->Surface, &present_modes[0], IM_COUNTOF(present_modes));

		// Create SwapChain, RenderPass, Framebuffer, etc.
		IM_ASSERT(g_MinImageCount >= 2);
		// ImGui_ImplVulkanH_CreateOrResizeWindow(
		// 	m_context->GetInstance(),
		// 	m_context->GetPhysicalDevice(),
		// 	m_device,
		// 	wd,
		// 	m_context->GetGraphicsFamilyIndex(),
		// 	nullptr,
		// 	windowSize.x,
		// 	windowSize.y,
		// 	g_MinImageCount,
		// 	0
		// );

		windowContext->SetupIMGUI(wd);

		return true;
	}

	void CleanupContext() override {
		if (m_descriptorPool != VK_NULL_HANDLE) {
			vkDestroyDescriptorPool(m_device, m_descriptorPool, nullptr);
			m_descriptorPool = VK_NULL_HANDLE;
		}

		if (m_device != VK_NULL_HANDLE) {
			vkDestroyDevice(m_device, nullptr);
			m_device = nullptr;
		}
	}

private:
	VkSurfaceKHR m_surface{nullptr};

	andromeda::graphics::VulkanContext* m_context{nullptr};
	VkDevice m_device{VK_NULL_HANDLE};
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
