#ifndef VULKAN_INST_H
#define VULKAN_INST_H
// #define VMA_IMPLEMENTATION
// #include "vk_mem_alloc.h"

#include "Engine/Graphics/GraphicsContext.h"
#include <SDL3/SDL.h>
#include <vulkan/vulkan.h>

namespace andromeda::graphics {
	struct VulkanQueueFamily {
		int32_t queueIndex{-1};
		VkQueueFlags queueFlags{0};
	};

	struct VulkanQueueFamilies {
		VulkanQueueFamily graphics{};
	};

	// The application-wide context for Vulkan
	class VulkanContext {
	public:
		VulkanContext();
		~VulkanContext();
		NO_COPY(VulkanContext)

		constexpr static uint32_t VulkanVersion{VK_API_VERSION_1_4};

		bool InitVulkan();
		void Shutdown();

		constexpr VkInstance GetInstance() const {
			return m_instance;
		}

		constexpr int32_t GetGraphicsFamilyIndex() const {
			return m_graphicsFamilyIndex;
		}

		constexpr VkPhysicalDevice GetPhysicalDevice() const {
			return m_physicalDevice;
		}

	protected:
		friend class VulkanWindowContext;


	private:
		VkInstance m_instance{VK_NULL_HANDLE};
		VkPhysicalDevice m_physicalDevice{VK_NULL_HANDLE};

		int32_t m_graphicsFamilyIndex{-1};

		bool CreateVulkanInstance();

		VkPhysicalDevice SelectPhysicalDevice();

		int32_t SelectGraphicsQueueFamilyIndex();
	};
} // namespace andromeda::graphics

#endif