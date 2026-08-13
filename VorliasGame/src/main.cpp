
#define ANDROMEDA_INTERNAL 1
#define ANDROMEDA_OPENGL 1
#include "Engine/Main.h"
#include "Engine/Window.h"
#include "Engine/Common.h"
#include "Engine/Engine.h"
#include "Engine/Graphics/Vulkan/VulkanRendererAPI.h"
#include "Engine/Graphics/OpenGL/OpenGLRenderer.h"

#include "Engine/Graphics/Vulkan/VulkanWindowContext.h"

#include "Engine/Graphics/OpenGL/OpenGLShader.h"
#include "Engine/Graphics/Vulkan/VulkanShader.h"

#include "Engine/IMGUI.h"
using namespace andromeda;

class TestApplication : public Application {
	bool Initialize() override {
		CreateWindow(WindowOptions());

		shaders.LoadShaderFromFiles("Main", "VorliasEngine/src/Shaders/shader.vert.spv", "VorliasEngine/src/Shaders/shader.frag.spv");

		return true;
	}
};


Application* ApplicationMain(const ApplicationInit& init) {
	if (contains(init.args, "-opengl")) {
		init.renderer = graphics::API::OpenGL;
	} else if (contains(init.args, "-vulkan")) {
		init.renderer = graphics::API::Vulkan;
	} else if (contains(init.args, "-headless")) {
		init.renderer = graphics::API::None;
	}

	return new TestApplication();
}


// namespace AN = andromeda;
// namespace AG = andromeda::graphics;

// #define DEBUG_VULKAN 1

// static AN::graphics::OpenGLRenderer opengl{};
// static AN::graphics::VulkanRenderer vulkan{};

// int main() {
// 	AN::Window wind = AN::Window(AN::WindowOptions());
// 	AG::ShaderLibrary shaders;

// #if DEBUG_VULKAN
// 	auto& engine = AN::Engine::GetInstance();

// 	if (!wind.Initialize(&vulkan)) {
// 		return 1;
// 	}

// 	auto vkWContext = static_cast<AN::graphics::VulkanWindowContext*>(wind.GetGraphicsContext());
// 	auto device = vkWContext->GetDevice();

// 	AN::graphics::VulkanShader vulkanShader(device);
// 	vulkanShader.LoadFromFile("VorliasEngine/src/Shaders/shader.vert.spv", AN::graphics::ShaderType::Vertex);
// 	vulkanShader.LoadFromFile("VorliasEngine/src/Shaders/shader.frag.spv", AN::graphics::ShaderType::Fragment);
// #else
// 	if (!wind.Initialize(&opengl)) {
// 		return 1;
// 	}

// 	AN::graphics::OpenGLShader glShaderTest;
// 	glShaderTest.LoadFromFile("VorliasEngine/src/Shaders/shader.vert.spv", AN::graphics::ShaderType::Vertex);
// 	glShaderTest.LoadFromFile("VorliasEngine/src/Shaders/shader.frag.spv", AN::graphics::ShaderType::Fragment);
// 	glShaderTest.LinkProgram();
// #endif


// 	// {
// 	// 	auto vkWContext = static_cast<AN::graphics::VulkanWindowContext*>(wind.GetGraphicsContext());
// 	// 	auto device = vkWContext->GetDevice();

// 	// 	AN::graphics::VulkanShader vkShader(device, AN::graphics::ShaderType::Vertex);
// 	// 	vkShader.LoadFromFile("VorliasEngine/src/Shaders/shader.vert");
// 	// }

// 	while (!wind.HasRequestedExit()) {
// 		SDL_Event e;
// 		while (SDL_PollEvent(&e)) {
// 			switch (e.type) {
// 				case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
// 					wind.Close();
// 					break;
// 			}
// 		}
// 	}

// #if DEBUG_VULKAN
// 	vulkanShader.Destroy();
// #else
// #endif

// 	wind.Shutdown();



// #if DEBUG_VULKAN
// 	vulkan.Shutdown();
// #else
// 	opengl.Shutdown();
// #endif
// 	SDL_Quit();

// 	return 0;
// }