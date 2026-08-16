
// #define ANDROMEDA_INTERNAL 1
#define ANDROMEDA_OPENGL 1
#include "Engine/Main.h"
#include "Engine/ObjectPool.h"
#include "Engine/Window.h"
#include "Engine/Common.h"
#include "Engine/File.h"
#include "Engine/Engine.h"
#include "Engine/Graphics/Vulkan/VulkanRendererAPI.h"
#include "Engine/Graphics/OpenGL/OpenGLRenderer.h"

#include "Engine/Graphics/Vulkan/VulkanWindowContext.h"

#include "Engine/Graphics/OpenGL/OpenGLShader.h"
#include "Engine/Graphics/Vulkan/VulkanShader.h"

#include "Engine/Luau/LuauScript.h"
#include "Engine/Luau/Task.h"
#include "Engine/Luau/LuauRuntime.h"

#include <thread>
#include <chrono>

using namespace std::string_literals;
using namespace std::chrono_literals;
using namespace std::string_view_literals;

#include "Engine/IMGUI.h"
using namespace andromeda;

class TestApplication : public Application {
public:
	TestApplication() {
		SetDataPath("Vorlias/src");
		SetPersistentDataPath(GetPersistentDataPath() / "Test");
	}

	bool Initialize() override {
		auto testScript = R"(
			print("Hello 1");

			task.spawn(function()
				task.wait(1);
				print("hello after 1 second")
			end)

			task.wait(2);
			print("hello after 2 seconds");
		)";
		
		auto script2 = LuauScript::CreateScript("task.wait(5); print('hi dere');", "test2.luau");
		auto script = LuauScript::CreateScript(testScript, "test.luau");

		luau.ExecuteScript(script);
		// luau.ExecuteScript(script2);

		// CreateWindow(WindowOptions());
		return true;
	}

	void Update(float dt) override {
		luau.Update(dt); // ezpz
		// std::cout << "do thing " << GetElapsedTime() << std::endl;
	}
private:
	LuauRuntime luau;
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