#include "Engine/Graphics/Vulkan/VulkanInstance.h"
#include "Engine/Log.h"
#define VOLK_IMPLEMENTATION
#include <volk.h>
#include <SDL3/SDL_vulkan.h>

namespace andromeda::graphics {
	bool VulkanContext::InitVulkan() {
		if (!CreateVulkanInstance()) {
			andromeda::error("Failed to create Vulkan Instance");
			return false;
		}

		return true;
	}

	void VulkanContext::Shutdown() {
		if (instance != VK_NULL_HANDLE) {
			vkDestroyInstance(instance, nullptr);
		}

		volkFinalize();
	}

	bool VulkanContext::CreateVulkanInstance() {
		// if (volkInitialize() != VK_SUCCESS) {
		// 	return false;
		// }

		// VkApplicationInfo appInfo{.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO, .pApplicationName = "Test", .apiVersion = VulkanVersion};
		// uint32_t instExtCount = 0;
		// const char* const* extensions = SDL_Vulkan_GetInstanceExtensions(&instExtCount);
		// std::vector<const char*> requestedExtensions = {VK_EXT_DEBUG_UTILS_EXTENSION_NAME};

		// for (int i = 0; i < instExtCount; i++) {
		// 	requestedExtensions.push_back(extensions[i]);
		// }

		// std::vector<const char*> requestedLayers = {"VK_LAYER_KHRONOS_validation"};

		// VkInstanceCreateInfo instCreateInfo{
		// 	.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		// 	.pNext = nullptr, // &debugInfo,
		// 	.pApplicationInfo = &appInfo,
		// 	.enabledLayerCount = static_cast<uint32_t>(requestedLayers.size()),
		// 	.ppEnabledLayerNames = requestedLayers.data(),
		// 	.enabledExtensionCount = static_cast<uint32_t>(requestedExtensions.size()),
		// 	.ppEnabledExtensionNames = requestedExtensions.data(),
		// };

		// if (vkCreateInstance(&instCreateInfo, nullptr, &instance) != VK_SUCCESS) {
		// 	return false;
		// }

		// volkLoadInstance(instance);
		return true;
	}
} // namespace andromeda::graphics