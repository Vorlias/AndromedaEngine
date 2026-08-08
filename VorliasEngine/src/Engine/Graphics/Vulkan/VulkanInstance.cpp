#include "Engine/Graphics/Vulkan/VulkanInstance.h"
#include "Engine/Log.h"
#define VOLK_IMPLEMENTATION
#include <volk.h>
#include <SDL3/SDL_vulkan.h>
#include "VulkanUtils.h"
#include <algorithm>
using namespace andromeda::graphics;

namespace andromeda::graphics {
	static VulkanContext* s_context;

	VulkanContext::VulkanContext() {
		if (s_context != nullptr) {
			andromeda::error("Can only have one instance of a VulkanContext per application!");
			abort();
			return;
		}

		s_context = this;
	}

	VulkanContext::~VulkanContext() {
		if (s_context == this)
			s_context = nullptr;
	}

	bool VulkanContext::InitVulkan() {
		if (!CreateVulkanInstance()) {
			andromeda::error("Failed to create Vulkan Instance");
			return false;
		}

		m_physicalDevice = SelectPhysicalDevice();
		if (m_physicalDevice == VK_NULL_HANDLE) {
			andromeda::error("Failed to select device");
			return false;
		}

		if (m_graphicsFamilyIndex = SelectGraphicsQueueFamilyIndex(); m_graphicsFamilyIndex < 0) {
			andromeda::error("Failed to select graphics queue");
			return false;
		}

		andromeda::trace("Created Andromeda Vulkan Context");
		return true;
	}

	void VulkanContext::Shutdown() {
		if (m_instance != VK_NULL_HANDLE) {
			trace("Destroyed Vulkan Instance");
			vkDestroyInstance(m_instance, nullptr);
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

		if (!SDL_WasInit(SDL_INIT_VIDEO)) {
			andromeda::warn("Cannot load vulkan without SDL_InitSubSystem(SDL_INIT_VIDEO)");
			return false;
		}

		VkInstanceCreateFlags flags{};
		VkApplicationInfo appInfo{.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO, .pApplicationName = "Test", .apiVersion = VulkanVersion};
		uint32_t instExtCount = 0;
		const char* const* extensions = SDL_WasInit(SDL_INIT_VIDEO) ? SDL_Vulkan_GetInstanceExtensions(&instExtCount) : nullptr;
		std::vector<const char*> requestedExtensions = {VK_EXT_DEBUG_UTILS_EXTENSION_NAME};

		for (int i = 0; i < instExtCount; i++) {
			requestedExtensions.push_back(extensions[i]);
		}


		// Stuff for IMGUI
		auto properties = vku::GetExtensionProperties();
		if (vku::HasExtension(properties, VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME)) {
			requestedExtensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
		}
#ifdef VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME
		if (vku::HasExtension(properties, VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME)) {
			requestedExtensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
			flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
		}
#endif

#if ANDROMEDA_INTERNAL
		for (auto& extension : requestedExtensions) {
			std::cout << COLOR_YELLOW << "[Vulkan] Request extension " << extension << COLOR_RESET << std::endl;
		}
#endif

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
			.flags = flags,
			.pApplicationInfo = &appInfo,
			.enabledLayerCount = static_cast<uint32_t>(requestedLayers.size()),
			.ppEnabledLayerNames = requestedLayers.data(),
			.enabledExtensionCount = static_cast<uint32_t>(requestedExtensions.size()),
			.ppEnabledExtensionNames = requestedExtensions.data(),
		};


		if (vkCreateInstance(&instCreateInfo, nullptr, &m_instance) != VK_SUCCESS) {
			andromeda::error("Could not create vulkan instance");
			return false;
		}

		volkLoadInstance(m_instance);
		return true;
	}

	VkPhysicalDevice VulkanContext::SelectPhysicalDevice() {
		uint32_t gpu_count;
		std::vector<VkPhysicalDevice> gpus;

		VkResult err = vkEnumeratePhysicalDevices(m_instance, &gpu_count, nullptr);
		if (err != VK_SUCCESS) {
			andromeda::error("Could not query physical devices");
			return VK_NULL_HANDLE;
		}

		gpus.resize(gpu_count);
		err = vkEnumeratePhysicalDevices(m_instance, &gpu_count, gpus.data());

		for (VkPhysicalDevice& device : gpus) {
			VkPhysicalDeviceProperties properties;
			vkGetPhysicalDeviceProperties(device, &properties);
			if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
				andromeda::print("Physical Device: " + std::string(properties.deviceName));
				return device;
			}
		}

		if (gpu_count > 0) {
			andromeda::print("Physical Device: Fallback (Integrated?)");
			return gpus[0];
		}

		return VK_NULL_HANDLE;
	}
} // namespace andromeda::graphics

int32_t andromeda::graphics::VulkanContext::SelectGraphicsQueueFamilyIndex() {
	uint32_t selectedFamilyIndex = UINT32_MAX;
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties2(m_physicalDevice, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties2> queueFamilyProperties(queueFamilyCount, {.sType = VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2});
	vkGetPhysicalDeviceQueueFamilyProperties2(m_physicalDevice, &queueFamilyCount, queueFamilyProperties.data());

	for (int familyIndex = 0; familyIndex < queueFamilyProperties.size(); familyIndex++) {
		const auto& props = queueFamilyProperties[familyIndex];
		if (props.queueFamilyProperties.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			return familyIndex;
		}
	}

	return -1;
}

VkDevice VulkanContext::CreateDevice(const VkVec<const char*>& extensions, const VkVec<VkDeviceQueueCreateInfo>& createInfos) const {
	VkDeviceCreateInfo createInfo{
		.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
		.queueCreateInfoCount = createInfos.count,
		.pQueueCreateInfos = createInfos.data,
		.enabledExtensionCount = extensions.count,
		.ppEnabledExtensionNames = extensions.data,
	};

	VkDevice device;
	if (vkCreateDevice(m_physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
		andromeda::error("Failed to create device from physical device");
		return VK_NULL_HANDLE;
	}

	return device;
}

VkQueue VulkanContext::CreateDeviceQueue(VkDevice device, uint32_t familyQueue, uint32_t queueIndex) const {
	VkQueue queue;
	vkGetDeviceQueue(device, familyQueue, queueIndex, &queue);
	return queue;
}