#define USE_IMGUI 1
#include "VulkanPlayground.h"
#include "Engine/Graphics/Vulkan/VulkanRendererAPI.h"
#include "Engine/Graphics/Vulkan/VulkanWindowContext.h"
#include "Engine/Graphics/Vulkan/VulkanIMGUI.h"
#include "Engine/Graphics/Vulkan/VulkanRenderTexture.h"

#include "Engine/Graphics/Vulkan/VulkanBase.h"

#include "SDL3/SDL.h"


#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl3.h"
#include "imgui/imgui_impl_vulkan.h"

static andromeda::graphics::VulkanRenderer s_renderer;
static andromeda::VulkanIMGUI* s_imgui;
static andromeda::WindowOptions s_windowOptions("Andromeda Vulkan Playground", andromeda::Vector2u(1920, 1080));

VulkanApplication::VulkanApplication() : window(s_windowOptions) {}

bool VulkanApplication::Initialize() {
	using namespace andromeda::graphics;

	if (!SDL_Init(SDL_INIT_VIDEO))
		return false;
	andromeda::initializeLogger("./VulkanPlayground.log");

	if (!s_renderer.Initialize())
		return false;



	bool windowInit = window.Initialize(&s_renderer);

	if (!windowInit)
		return false;

#if USE_IMGUI
	{
		auto vk = s_renderer.GetContext();
		auto ctx = static_cast<VulkanWindowContext*>(window.GetGraphicsContext());

		s_imgui = new andromeda::VulkanIMGUI(window.GetHandle(), vk, ctx);
		s_imgui->Initialize();

		rt = new VulkanRenderTexture(VulkanRenderTexture::RENDER_TEXTURE_IMGUI);
		rt->Create(vk, ctx, 1920, 1080);

		ctx->SetTargetRenderTexture(rt);
	}
#endif

	return true;
}

std::string EventStr(Uint32 eventType) {
	switch (eventType) {
		case SDL_EVENT_KEYBOARD_ADDED:
			return "KeyboardAdded";
		case SDL_EVENT_MOUSE_ADDED:
			return "MouseAdded";
		case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
			return "WindowPixelSizeChanged";
		case SDL_EVENT_WINDOW_SHOWN:
			return "WindowShown";
		case SDL_EVENT_WINDOW_EXPOSED:
			return "WindowExposed";
		default:
			return std::to_string((int)eventType);
	}
}

void VulkanApplication::Run() {
	SDL_InitSubSystem(SDL_INIT_GAMEPAD);

	while (!window.HasRequestedExit()) {
		SDL_Event e;
		while (window.PollSDLEvent(&e)) {
#if USE_IMGUI
			s_imgui->ProcessEvent(e);
#endif

			switch (e.type) {
				case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
					window.Close();
					break;
				case SDL_EVENT_QUIT:
					andromeda::print("Quitting application as requested by user");
					window.Close();
					break;
				case SDL_EVENT_WINDOW_RESIZED:
					window.Resized(e.window.data1, e.window.data2);
#if USE_IMGUI
					s_imgui->Resize(e.window.data1, e.window.data2);
#endif
					break;
			}
		}



		Update();
		Render();
	}
}

void VulkanApplication::Update() {}

void VulkanApplication::Render() {
#if USE_IMGUI
	s_imgui->NewFrame();
#endif


	auto ctx = static_cast<andromeda::graphics::VulkanWindowContext*>(window.GetGraphicsContext());
	ctx->BeforeRender();

	ctx->RenderPrepare();
#if USE_IMGUI
	ImGui::Begin("Render Target Test");
	{
		ImVec2 avail = ImGui::GetContentRegionAvail();
		int width = static_cast<int>(avail.x);
		int height = static_cast<int>(avail.y);

		if (width > 0 && height > 0) {
			rt->Resize(width, height);
			ImGui::Image(rt->GetImGuiTexture(), avail);
		}
	}
	ImGui::End();
#endif

	ctx->RenderDraw();
#if USE_IMGUI
	s_imgui->Render();
#endif

	ctx->RenderPresent();
}

void VulkanApplication::Shutdown() {
#if USE_IMGUI
	rt->Destroy();
	delete rt;

	s_imgui->Shutdown();
#endif

	window.Shutdown();
	s_renderer.Shutdown();
	SDL_Quit();
}