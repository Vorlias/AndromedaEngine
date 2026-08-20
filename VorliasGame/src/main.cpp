
// #define ANDROMEDA_INTERNAL 1
#define ANDROMEDA_OPENGL 1
#define ANDROMEDA_EDITOR 0

#include "SDL3/SDL.h"

#include "Engine/Engine.h"
#include "Engine/Graphics/Vulkan/VulkanInstance.h"
#include "Engine/Graphics/Vulkan/VulkanRendererAPI.h"

#include "Engine/Window.h"
using namespace andromeda;

#include "VulkanPlayground.h"
#include "LuauTesting.h"

int main() {
	LuauApplication* app = new LuauApplication();
	auto& engine = Engine::GetInstance();
	engine.Run(app);

	// VulkanApplication app;
	// if (app.Initialize()) {
	// 	app.Run();
	// }

	// app.Shutdown();
	return 0;
}