#include "VulkanPlayground.h"
#include "Engine/Graphics/Vulkan/VulkanRendererAPI.h"
#include "Engine/Graphics/Vulkan/VulkanWindowContext.h"
#include "Engine/Graphics/Vulkan/VulkanIMGUI.h"

#include "SDL3/SDL.h"


#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl3.h"
#include "imgui/imgui_impl_vulkan.h"

static andromeda::graphics::VulkanRenderer s_renderer;
static andromeda::VulkanIMGUI* s_imgui;

VulkanApplication::VulkanApplication() : window(andromeda::WindowOptions()) {}

bool VulkanApplication::Initialize() {
	if (!SDL_Init(SDL_INIT_VIDEO))
		return false;
	// andromeda::initializeLogger("./VulkanPlayground.log");



	if (!s_renderer.Initialize())
		return false;


	bool windowInit = window.Initialize(&s_renderer);

	if (!windowInit)
		return false;

	{
		auto vk = s_renderer.GetContext();
		auto ctx = static_cast<andromeda::graphics::VulkanWindowContext*>(window.GetGraphicsContext());

		s_imgui = new andromeda::VulkanIMGUI(window.GetHandle(), vk, ctx);
		s_imgui->Initialize();
	}

	return true;
}

void VulkanApplication::Run() {
	while (!window.HasRequestedExit()) {
		SDL_Event e;
		ImGui_ImplSDL3_ProcessEvent(&e);
		while (window.PollSDLEvent(&e)) {
			switch (e.type) {
				case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
					window.Close();
					break;
			}
		}

		Update();
		Render();
	}
}

void VulkanApplication::Update() {
	
}

void VulkanApplication::Render() {
	s_imgui->NewFrame();

	auto ctx = static_cast<andromeda::graphics::VulkanWindowContext*>(window.GetGraphicsContext());
	ctx->Prepare();
	ctx->Render();
	ctx->Present();

	ImGui::ShowDemoWindow();

	s_imgui->Render();
}

void VulkanApplication::Shutdown() {
	// ImGui_ImplVulkan_Shutdown();
	// ImGui_ImplSDL3_Shutdown();
	s_imgui->Shutdown();

	window.Shutdown();
	s_renderer.Shutdown();
	SDL_Quit();
}