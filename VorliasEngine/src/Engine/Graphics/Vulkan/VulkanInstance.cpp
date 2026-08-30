#include "Engine/Graphics/Vulkan/VulkanInstance.h"
#include "Engine/Log.h"
#include "Engine/Graphics/Vulkan/VulkanBase.h"
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

		// if (!CreateDevice(m_graphicsFamilyIndex)) {
		// 	ANDROMEDA_ERR("Failed to create device with familyIndex of {}", m_graphicsFamilyIndex);
		// 	return false;
		// }

		// if (!InitializeVMA()) {
		// 	ANDROMEDA_ERR("Failed to initialize VMA");
		// 	return false;
		// }

		ANDROMEDA_VK_CHECK(CreateDevice(m_graphicsFamilyIndex), "Failed to create device with graphicsFamilyIndex of {}", m_graphicsFamilyIndex);
		ANDROMEDA_VK_CHECK(InitializeVMA(), "Failed to initialize VMA");

		andromeda::trace("Created Andromeda Vulkan Context");
		return true;
	}

	void VulkanContext::Shutdown() {
		if (m_allocator) {
			vmaDestroyAllocator(m_allocator);
		}

		if (m_device != VK_NULL_HANDLE) {
			vkDestroyDevice(m_device, nullptr);
		}

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
			std::cout << COLOR_CYAN << "[Vulkan] Request extension " << extension << COLOR_RESET << std::endl;
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

	bool VulkanContext::CreateDevice(uint32_t graphicsQueueIndex) {
		VkPhysicalDeviceVulkan14Features supportedFeatures14{.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_4_FEATURES, .pNext = nullptr};
		VkPhysicalDeviceVulkan13Features supportedFeatures13{
			.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES, .pNext = &supportedFeatures14
		};
		VkPhysicalDeviceVulkan12Features supportedFeatures12{
			.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES, .pNext = &supportedFeatures13
		};
		VkPhysicalDeviceFeatures2 supportedFeatures{.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2, .pNext = &supportedFeatures12};

		vkGetPhysicalDeviceFeatures2(m_physicalDevice, &supportedFeatures);

		// Check if what we need is supported
		if (!supportedFeatures13.dynamicRendering || !supportedFeatures13.synchronization2 || !supportedFeatures13.synchronization2 ||
		    !supportedFeatures12.timelineSemaphore) {
			andromeda::error("Physical device does not meet the feature requirements");
			return false;
		}

		// Then providing a set of features for the device to use - only ones planned to be used
		// Good practice is to keep it separate.
		VkPhysicalDeviceVulkan14Features features14{.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_4_FEATURES, .pNext = nullptr};
		VkPhysicalDeviceVulkan13Features features13{
			.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES,
			.pNext = &features14,
			.synchronization2 = VK_TRUE,
			.dynamicRendering = VK_TRUE
		};
		VkPhysicalDeviceVulkan12Features features12{
			.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES, .pNext = &features13, .timelineSemaphore = VK_TRUE
		};

		VkPhysicalDeviceFeatures2 features{.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2, .pNext = &features12};

		// request the queues to be used
		std::vector<float> queuePriorities{1.0f};
		VkDeviceQueueCreateInfo graphicsQueueInfo{
			.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
			.queueFamilyIndex = graphicsQueueIndex,
			.queueCount = 1,
			.pQueuePriorities = queuePriorities.data(),
		};

		// device-specific extensions
		const std::vector<const char*> deviceExtensions{// Companion to surface extension
		                                                VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

		VkDeviceCreateInfo devCreateInfo{
			.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
			.pNext = &features,
			.queueCreateInfoCount = 1,
			.pQueueCreateInfos = &graphicsQueueInfo,
			.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size()),
			.ppEnabledExtensionNames = deviceExtensions.data(),
			.pEnabledFeatures = nullptr, // better to use pNext linked list for features
		};

		if (vkCreateDevice(m_physicalDevice, &devCreateInfo, nullptr, &m_device) != VK_SUCCESS) {
			return false;
		}

		vkGetDeviceQueue(m_device, graphicsQueueIndex, 0, &m_graphicsQueue);
		if (!m_graphicsQueue) {
			andromeda::error("Couldn't get the graphics queue");
			return false;
		}

		return true;
	}

	bool VulkanContext::InitializeVMA() {
		VmaVulkanFunctions vmaFuncInfo{};
		VmaAllocatorCreateInfo vmaAllocInfo{
			.flags = 0,
			.physicalDevice = m_physicalDevice,
			.device = m_device,
			.pVulkanFunctions = &vmaFuncInfo,
			.instance = m_instance,
			.vulkanApiVersion = VulkanContext::VulkanVersion,
		};

		// vma can import directly from volk
		vmaImportVulkanFunctionsFromVolk(&vmaAllocInfo, &vmaFuncInfo);

		if (vmaCreateAllocator(&vmaAllocInfo, &m_allocator) != VK_SUCCESS) {
			return false;
		}

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