#include <vector>
#include "VulkanUtils.h"

const std::vector<VkExtensionProperties> vku::GetExtensionProperties() {
	uint32_t properties_count;
	std::vector<VkExtensionProperties> properties;
	vkEnumerateInstanceExtensionProperties(nullptr, &properties_count, nullptr);
	properties.resize(properties_count);
	vkEnumerateInstanceExtensionProperties(nullptr, &properties_count, properties.data());
	return properties;
}

bool vku::HasExtension(const std::vector<VkExtensionProperties>& extensions, const std::string& name) {
    for (auto& ext : extensions) {
        if (name == ext.extensionName) return true;
    }

    return false;
}
