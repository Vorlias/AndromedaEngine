#pragma once
#include <volk.h>

namespace vku {
	const std::vector<VkExtensionProperties> GetExtensionProperties();
    bool HasExtension(const std::vector<VkExtensionProperties>& extensions, const std::string& name);
} // namespace vku