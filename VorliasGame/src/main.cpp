
#define ANDROMEDA_INTERNAL 1
#include "Engine/Window.h"
#include "Engine/Graphics/Vulkan/VulkanRendererAPI.h"
#include "Engine/Graphics/Vulkan/VulkanInstance.h"
#include "Engine/IMGUI.h"


namespace AN = andromeda;

static andromeda::graphics::VulkanRenderer vr{};
int main() {
	AN::Window window = AN::Window(AN::WindowOptions());

	if (!SDL_Init(SDL_INIT_VIDEO)) return -1;
	

	if (!vr.Initialize()) {
		vr.Shutdown();
		return 1;
	}

	if (!window.Initialize(&vr)) {
		vr.Shutdown();
		return 2;
	}

	auto imWindow = AN::ImWindowContext::Create(&vr);
	imWindow->Initialize(window);
	
	while (!window.HasRequestedExit()) {
		SDL_Event e;
		while (window.PollSDLEvent(&e)) {
			switch (e.type) {
				case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
					window.Close();
					break;
			}
		}

		imWindow->NewFrame();
		ImGui::ShowDemoWindow();
		imWindow->Render();
	}

	window.Shutdown();
	imWindow->Shutdown();
	vr.Shutdown();

	SDL_Quit();
	return 0;
}