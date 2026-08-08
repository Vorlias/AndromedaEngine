#pragma once
#include <volk.h>
#include "Engine/Graphics/Vulkan/VulkanInstance.h"
#include "Engine/Log.h"

#define VK_OK(v) v == VK_SUCCESS
#define VK_ERR(v) v != VK_SUCCESS
#define VK_CHECK(v) \
	{ \
		VkBool32 res = v; \
		if (res != VK_SUCCESS) { \
			andromeda::error("Check " #v "failed") \
		} \
	}

#define VK_CHECK_ELSE_RETURN(V, R) \
	{ \
		VkBool32 res = V; \
		if (res != VK_SUCCESS) { \
			andromeda::error("Check " #V "failed") \
			return R; \
		} \
	}

namespace vku {
	const std::vector<VkExtensionProperties> GetExtensionProperties();
	bool HasExtension(const std::vector<VkExtensionProperties>& extensions, const std::string& name);

	// std::string to_string(VkBool32 value) {
	// 	switch (value) {
	// 		case VK_SUCCESS:
	// 			return "Success";
	// 		default:
	// 			return std::to_string(value);
	// 	}
	// }

	static VkDevice CreateDevice(
		andromeda::graphics::VulkanContext* context,
		const char* const* deviceExtensions,
		uint32_t extensionsCount,
		VkDeviceQueueCreateInfo* queueCreateInfos,
		uint32_t queueCreateInfosCount
	) {
		VkDeviceCreateInfo createInfo{
			.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
			.queueCreateInfoCount = queueCreateInfosCount,
			.pQueueCreateInfos = queueCreateInfos,
			.enabledExtensionCount = (uint32_t)extensionsCount,
			.ppEnabledExtensionNames = deviceExtensions,
		};

		VkDevice device;
		if (vkCreateDevice(context->GetPhysicalDevice(), &createInfo, nullptr, &device) != VK_SUCCESS) {
			andromeda::error("Failed to create device from physical device");
			return VK_NULL_HANDLE;
		}

		// return VK_NULL_HANDLE;
		return device;
	}
} // namespace vku