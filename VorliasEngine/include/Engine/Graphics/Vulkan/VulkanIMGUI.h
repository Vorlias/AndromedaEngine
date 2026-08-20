#pragma once
#include "Engine/IMGUI.h"
#include "Engine/Graphics/Vulkan/VulkanRendererAPI.h"
#include "Engine/Graphics/Vulkan/VulkanWindowContext.h"
#include "Engine/Graphics/Vulkan/VulkanInstance.h"

#include <SDL3/SDL.h>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl3.h"
#include "imgui/imgui_impl_vulkan.h"

namespace andromeda {
	class VulkanIMGUI {
	public:
		VulkanIMGUI(SDL_Window* window, graphics::VulkanContext* context, graphics::VulkanWindowContext* windowCtx)
			: m_vk(context), m_vkWindow(windowCtx), m_window(window) {}

		void Initialize();

		void UpdateSwapchain();
		void NewFrame();
		void Render();
		void Shutdown();
	private:
		void CreateCommandBuffers();
	private:
		graphics::VulkanContext* m_vk;
		graphics::VulkanWindowContext* m_vkWindow;
		SDL_Window* m_window;

		ImGui_ImplVulkan_InitInfo* m_initInfo;
		ImGui_ImplVulkanH_Window* m_windowData;
		ImGui_ImplVulkan_PipelineInfo* m_pipeline;

		std::vector<VkCommandBuffer> m_commandBuffers;
	};
} // namespace andromeda