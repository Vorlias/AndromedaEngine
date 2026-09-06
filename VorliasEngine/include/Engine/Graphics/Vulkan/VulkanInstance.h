#ifndef VULKAN_INST_H
#define VULKAN_INST_H

#include "Engine/Log.h"
#include "Engine/Graphics/GraphicsContext.h"
#include <SDL3/SDL.h>
#include <imgui/imgui.h>
#include <volk.h>
#include "Engine/Graphics/Vulkan/VulkanBase.h"

namespace andromeda::graphics {
	// Data structure that contains an array and size portion that can be passed directly to vulkan
	template<typename T>
	struct VkVec {
		const T* data{nullptr};
		uint32_t count{0};

		VkVec(T* data, uint32_t size) : data(data), count(size) {}
		VkVec(std::vector<T> vector) : data(vector.data()), count(vector.size()) {}
		VkVec(ImVector<T> vector) : data(vector.Data), count((uint32_t)vector.Size) {}
		VkVec(std::pair<T*, uint32_t> pair) : data(pair.first), count(pair.second) {}
	};

	struct GPUTexture {
		uint32_t imageId = 0;
		uint32_t samplerId = 0;
	};

	struct GPUBuffer {
		VkBuffer vkBuffer = VK_NULL_HANDLE;
		uint32_t deviceAddress = 0;
		VmaAllocation allocation = VK_NULL_HANDLE;
	};

	struct GPUImage {
		VmaAllocation allocation = VK_NULL_HANDLE;
		VkImage image = VK_NULL_HANDLE;
		VkImageView imageView = VK_NULL_HANDLE;
	};

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

		[[nodiscard]] bool InitVulkan();
		void Shutdown();

		// Create a logical device instance for the physical device
		VkDevice CreateDevice(const VkVec<const char*>& extensions, const VkVec<VkDeviceQueueCreateInfo>& createInfos) const;
		VkQueue CreateDeviceQueue(VkDevice device, uint32_t familyQueue, uint32_t queueIndex) const;

		[[nodiscard]] constexpr VkInstance GetInstance() const {
			return m_instance;
		}

		[[nodiscard]] constexpr VkDevice GetDevice() const {
			return m_device;
		}

		[[nodiscard]] constexpr int32_t GetGraphicsFamilyIndex() const {
			return m_graphicsFamilyIndex;
		}

		[[nodiscard]] constexpr VkPhysicalDevice GetPhysicalDevice() const {
			return m_physicalDevice;
		}

		[[nodiscard]] constexpr VmaAllocator GetAllocator() const {
			return m_allocator;
		}

		[[nodiscard]] constexpr VkQueue GetGraphicsQueue() const {
			return m_graphicsQueue;
		}
	protected:
		friend class VulkanWindowContext;
	private:
		bool CreateVulkanInstance();
		VkPhysicalDevice SelectPhysicalDevice();
		int32_t SelectGraphicsQueueFamilyIndex();

		bool CreateDevice(uint32_t graphicsQueueIndex);
		bool InitializeVMA();
	private:
		VkInstance m_instance{VK_NULL_HANDLE};
		VkPhysicalDevice m_physicalDevice{VK_NULL_HANDLE};

		VmaAllocator m_allocator = VK_NULL_HANDLE;

		VkDevice m_device = VK_NULL_HANDLE;
		VkQueue m_graphicsQueue = VK_NULL_HANDLE;

		int32_t m_graphicsFamilyIndex{-1};
	};
} // namespace andromeda::graphics

#endif