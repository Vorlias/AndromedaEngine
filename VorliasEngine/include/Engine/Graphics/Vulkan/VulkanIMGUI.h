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
	class VulkanIMGUI final : public IMGUI {
	public:
		VulkanIMGUI(SDL_Window* window, graphics::VulkanContext* context, graphics::VulkanWindowContext* windowCtx)
			: m_vk(context), m_vkWindow(windowCtx), IMGUI(window) {}

		void Initialize() override;
		void UpdateSwapchain() override;
		void Resize(int width, int height) override;
		void NewFrame() override;
		void Render() override;
		bool ProcessEvent(SDL_Event& e) override;
		void Shutdown() override;

		constexpr bool IsActive() const { return m_active; }
	private:
		void CreateCommandBuffers();
	private:
		bool m_active = false;

		graphics::VulkanContext* m_vk;
		graphics::VulkanWindowContext* m_vkWindow;

		ImGui_ImplVulkan_InitInfo* m_initInfo;
		ImGui_ImplVulkanH_Window* m_windowData;
		ImGui_ImplVulkan_PipelineInfo* m_pipeline;

		std::array<ImGui_ImplVulkanH_Frame, graphics::VulkanWindowContext::MaxFramesInFlight> m_frames;
	};
} // namespace andromeda