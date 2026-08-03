#include "Engine/Graphics/Vulkan/VulkanWindowContext.h"
#include "Engine/Log.h"
#include <volk.h>
#define VMA_IMPLEMENTATION
#include "vk_mem_alloc.h"
#include <SDL3/SDL_vulkan.h>
#include <spdlog/spdlog.h>

namespace andromeda::graphics {
	VulkanWindowContext::VulkanWindowContext(VulkanContext& vulkan, SDL_Window* window) : vulkan(vulkan), window(window) {
		andromeda::trace("Create window context");
	}

	void VulkanWindowContext::Initialize() {
		if (!CreateSurface())
			return;
		if (physicalDevice = FindPhysicalDevice(); !physicalDevice) {
			return;
		}

		uint32_t queueIndex;
		if (queueIndex = FindGraphicsQueue(); queueIndex == UINT32_MAX) {
			return;
		}

		if (!CreateDevice(queueIndex)) {
			return;
		}

		if (!InitializeVMA()) {
			return;
		}

		int width, height;
		SDL_GetWindowSize(window, &width, &height);
		if (!CreateSwapchain(width, height)) {
			return;
		}
	}

	void VulkanWindowContext::Shutdown() {
		DestroySwapchain();

		if (vmaAllocator) {
			vmaDestroyAllocator(vmaAllocator);
		}

		if (device != VK_NULL_HANDLE) {
			vkDestroyDevice(device, nullptr);
		}

		if (surface != VK_NULL_HANDLE) {
			andromeda::trace("Cleaned up surface");
			SDL_Vulkan_DestroySurface(vulkan.instance, surface, nullptr);
			surface = nullptr;
		}
	}

	bool VulkanWindowContext::CreateSurface() {
		if (!SDL_Vulkan_CreateSurface(window, vulkan.instance, nullptr, &surface)) {
			andromeda::warn("Could not create surface for window " + std::to_string(SDL_GetWindowID(window)));
			return false;
		}

		andromeda::trace("Create surface for window");
		return true;
	}

	bool VulkanWindowContext::InitializeVMA() {
		VmaVulkanFunctions vmaFuncInfo{};
		VmaAllocatorCreateInfo vmaAllocInfo{
			.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT,
			.physicalDevice = physicalDevice,
			.device = device,
			.pVulkanFunctions = &vmaFuncInfo,
			.instance = vulkan.instance,
			.vulkanApiVersion = VulkanContext::VulkanVersion,
		};

		// vma can import directly from volk
		vmaImportVulkanFunctionsFromVolk(&vmaAllocInfo, &vmaFuncInfo);

		if (vmaCreateAllocator(&vmaAllocInfo, &vmaAllocator) != VK_SUCCESS) {
			return false;
		}

		return true;
	}

	bool VulkanWindowContext::CreateSwapchain(int width, int height) {
		// Track swapchain size separate from window size
		swapchainWidth = width;
		swapchainHeight = height;

		// Ensure tracking apropriate number of images
		VkSurfaceCapabilitiesKHR surfaceCaps{};
		if (vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfaceCaps) != VK_SUCCESS) {
			andromeda::error("Could not get the surface capabilities");
			return false;
		}

		uint32_t requestedImageCount = std::max(2u, surfaceCaps.minImageCount);
		if (surfaceCaps.maxImageCount > 0) {
			requestedImageCount = std::min(requestedImageCount, surfaceCaps.maxImageCount);
		}

		VkSwapchainCreateInfoKHR swapchainCreateInfo{
			.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
			.surface = surface,
			.minImageCount = requestedImageCount,
			.imageFormat = swapchainFormat,
			.imageColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR, // makes sure colors are reproduced correctly
			.imageExtent{.width = swapchainWidth, .height = swapchainHeight}, // the dimensions of the images
			.imageArrayLayers = 1, // each swapchain image should be a single image
			.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, // we're just drawing
			.preTransform = surfaceCaps.currentTransform, // how to orient the image - we account for any custom user rotations etc.
			.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR, // ensure window is fully opaque, allowing transparency
			.presentMode = VK_PRESENT_MODE_FIFO_KHR, // how the presentation engine chooses which image to draw - standard guarantee is surface will
		                                             // have this and good w/ vsync
		};

		if (vkCreateSwapchainKHR(device, &swapchainCreateInfo, nullptr, &swapchain) != VK_SUCCESS) {
			return false;
		}

		// ask for the swapchain images
		uint32_t imageCount = 0;
		vkGetSwapchainImagesKHR(device, swapchain, &imageCount, nullptr);
		swapchainImages.resize(imageCount);
		vkGetSwapchainImagesKHR(device, swapchain, &imageCount, swapchainImages.data());
		swapchainImageViews.resize(imageCount);

		andromeda::trace("Created swapchain images count=" + std::to_string(imageCount));

		for (size_t i = 0; i < swapchainImages.size(); i++) {
			VkImageViewCreateInfo imgViewInfo{
				.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
				.image = swapchainImages[i],
				.viewType = VK_IMAGE_VIEW_TYPE_2D, // regular 2D image
				.format = swapchainFormat,
				.subresourceRange{
					// dictates what portion of the image to access
					.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
					.baseMipLevel = 0,
					.levelCount = 1, // mipMapLevelCount
					.baseArrayLayer = 0,
					.layerCount = 1,
				},
			};

			if (vkCreateImageView(device, &imgViewInfo, nullptr, &swapchainImageViews[i]) != VK_SUCCESS) {
				andromeda::error("Failed to create image view");
				return false;
			}
		}

		renderCompleteSemaphores.resize(swapchainImages.size());
		for (VkSemaphore& semaphore : renderCompleteSemaphores) {
			VkSemaphoreCreateInfo semaphoreInfo{.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};

			if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &semaphore) != VK_SUCCESS) {
				andromeda::error("Error creating the render-complete semaphore");
				return false;
			}
		}


		// swapchain images owned by OS, depth has to be created

		// Create depth image - ensure visibility order is maintained
		VkImageCreateInfo depthCreateInfo{
			.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
			.imageType = VK_IMAGE_TYPE_2D,
			.format = depthFormat,
			.extent{.width = swapchainWidth, .height = swapchainHeight, .depth = 1},
			.mipLevels = 1,
			.arrayLayers = 1, // single image depth buffer
			.samples = VK_SAMPLE_COUNT_1_BIT, // don't need to multisample
			.tiling = VK_IMAGE_TILING_OPTIMAL,
			.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
			.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
		};

		VmaAllocationCreateInfo allocInfo{
			.flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT,
			.usage = VMA_MEMORY_USAGE_AUTO,
		};

		if (vmaCreateImage(vmaAllocator, &depthCreateInfo, &allocInfo, &depthImage, &depthImageAllocation, nullptr) != VK_SUCCESS) {
			andromeda::error("Failed to allocate depth image");
			return false;
		}


		VkImageViewCreateInfo depthImgViewInfo{
			.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
			.image = depthImage,
			.viewType = VK_IMAGE_VIEW_TYPE_2D,
			.format = depthFormat,
			.subresourceRange{.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT, .levelCount = 1, .layerCount = 1}, // only depth data (only aspect)
		};

		if (vkCreateImageView(device, &depthImgViewInfo, nullptr, &depthImageView) != VK_SUCCESS) {
			andromeda::error("Error creating depth image view");
			return false;
		}

		return true;
	}

	void VulkanWindowContext::DestroySwapchain() {
		for (VkImageView swapchainImageView : swapchainImageViews) {
			vkDestroyImageView(device, swapchainImageView, nullptr);
		}
		swapchainImageViews.clear();

		for (VkSemaphore& semaphore : renderCompleteSemaphores) {
			vkDestroySemaphore(device, semaphore, nullptr);
		}
		renderCompleteSemaphores.clear();

		if (swapchain) {
			vkDestroySwapchainKHR(device, swapchain, nullptr);
			swapchain = nullptr;
		}

		if (depthImageView) {
			vkDestroyImageView(device, depthImageView, nullptr);
			vmaDestroyImage(vmaAllocator, depthImage, depthImageAllocation);
			depthImageView = nullptr;
		}
	}

	VkPhysicalDevice VulkanWindowContext::FindPhysicalDevice() {
		uint32_t physicalDeviceCount = 0;
		vkEnumeratePhysicalDevices(vulkan.instance, &physicalDeviceCount, nullptr);

		std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
		vkEnumeratePhysicalDevices(vulkan.instance, &physicalDeviceCount, physicalDevices.data());

		VkPhysicalDevice physicalDevice = nullptr;
		if (physicalDeviceCount) {
			physicalDevice = physicalDevices[0];

			// Find a dGPU
			for (auto& physicalDevice : physicalDevices) {
				VkPhysicalDeviceProperties props{};
				vkGetPhysicalDeviceProperties(physicalDevice, &props);

				if (props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
					this->physicalDevice = physicalDevice;
					spdlog::info("Using physical device {}", props.deviceName);
					break;
				}
			}
		}

		// Ensure the desired swapchain format is supported
		uint32_t formatCount = 0;
		vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);
		std::vector<VkSurfaceFormatKHR> surfaceFormats(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, surfaceFormats.data());

		bool formatSupported = false;
		for (const VkSurfaceFormatKHR& surfFormat : surfaceFormats) {
			if (surfFormat.format == swapchainFormat) {
				formatSupported = true;
				break;
			}
		}

		if (!formatSupported) {
			andromeda::error("Requested swapchain format is not supported by the surface");
			return nullptr;
		}


		return physicalDevice;
	}

	bool VulkanWindowContext::CreateDevice(uint32_t graphicsQueueIndex) {
		VkPhysicalDeviceVulkan14Features supportedFeatures14{.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_4_FEATURES, .pNext = nullptr};
		VkPhysicalDeviceVulkan13Features supportedFeatures13{
			.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES, .pNext = &supportedFeatures14
		};
		VkPhysicalDeviceVulkan12Features supportedFeatures12{
			.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES, .pNext = &supportedFeatures13
		};
		VkPhysicalDeviceFeatures2 supportedFeatures{.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2, .pNext = &supportedFeatures12};

		vkGetPhysicalDeviceFeatures2(physicalDevice, &supportedFeatures);

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

		if (vkCreateDevice(physicalDevice, &devCreateInfo, nullptr, &device) != VK_SUCCESS) {
			return false;
		}

		vkGetDeviceQueue(device, graphicsQueueIndex, 0, &graphicsQueue);
		if (!graphicsQueue) {
			andromeda::error("Couldn't get the graphics queue");
			return false;
		}

		return true;
	}

	uint32_t VulkanWindowContext::FindGraphicsQueue() {
		uint32_t selectedFamilyIndex = UINT32_MAX;
		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties2(physicalDevice, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties2> queueFamilyProperties(queueFamilyCount, {.sType = VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2});
		vkGetPhysicalDeviceQueueFamilyProperties2(physicalDevice, &queueFamilyCount, queueFamilyProperties.data());

		for (int familyIndex = 0; familyIndex < queueFamilyProperties.size(); familyIndex++) {
			// ensure it has presentation support
			VkBool32 hasPresentSupport = VK_FALSE;
			vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, familyIndex, surface, &hasPresentSupport);

			// ensure this is a GRAPHICS queue with presentation support
			const auto& props = queueFamilyProperties[familyIndex];
			if (props.queueFamilyProperties.queueFlags & VK_QUEUE_GRAPHICS_BIT && hasPresentSupport) {
				selectedFamilyIndex = familyIndex;
			}
		}

		return selectedFamilyIndex;
	}
} // namespace andromeda::graphics