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

		andromeda::print("Created Vulkan Context");
		return true;
	}

	void VulkanContext::Shutdown() {
		if (instance != VK_NULL_HANDLE) {
			trace("Destroyed Vulkan Instance");
			vkDestroyInstance(instance, nullptr);
		}

		volkFinalize();
	}

	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageTypes,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData
	) {
		switch (messageSeverity) {
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
				andromeda::error(pCallbackData->pMessage);
				break;
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
				andromeda::warn(pCallbackData->pMessage);
				break;
#if defined(ANDROMEDA_DEBUG)
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
				andromeda::print(pCallbackData->pMessage);
				break;
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
				andromeda::trace(pCallbackData->pMessage);
				break;
#endif
			default:
				break;
		}

		return VK_FALSE;
	}

	bool VulkanContext::CreateVulkanInstance() {
		if (volkInitialize() != VK_SUCCESS) {
			andromeda::error("Could not initialize Volk");
			return false;
		}

		VkApplicationInfo appInfo{.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO, .pApplicationName = "Test", .apiVersion = VulkanVersion};
		uint32_t instExtCount = 0;
		const char* const* extensions = SDL_Vulkan_GetInstanceExtensions(&instExtCount);
		std::vector<const char*> requestedExtensions = {VK_EXT_DEBUG_UTILS_EXTENSION_NAME};

		for (int i = 0; i < instExtCount; i++) {
			requestedExtensions.push_back(extensions[i]);
		}

		std::vector<const char*> requestedLayers = {"VK_LAYER_KHRONOS_validation"};

		// for the validation layer
		VkDebugUtilsMessengerCreateInfoEXT debugInfo{
			.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
			.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
		                       VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
			.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
			.pfnUserCallback = debugCallback,
		};

		VkInstanceCreateInfo instCreateInfo{
			.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
			.pNext = &debugInfo,
			.pApplicationInfo = &appInfo,
			.enabledLayerCount = static_cast<uint32_t>(requestedLayers.size()),
			.ppEnabledLayerNames = requestedLayers.data(),
			.enabledExtensionCount = static_cast<uint32_t>(requestedExtensions.size()),
			.ppEnabledExtensionNames = requestedExtensions.data(),
		};

		if (vkCreateInstance(&instCreateInfo, nullptr, &instance) != VK_SUCCESS) {
			andromeda::error("Could not create vulkan instance");
			return false;
		}

		volkLoadInstance(instance);
		trace("Created Vulkan Instance");
		return true;
	}
} // namespace andromeda::graphics