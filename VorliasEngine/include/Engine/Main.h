#pragma once
#include "Application.h"
#include "Engine/Engine.h"
#include "Engine/Log.h"
#include <exception>
#include <iostream>
// #include <spdlog/spdlog.h>

namespace ENGINE_NS {
	struct ApplicationEntryPoint {
	};
} // namespace ENGINE_NS

ENGINE_NS::Application* ApplicationMain();

// Define an application class as the main
#define MAIN_APP(_APP) \
	ENGINE_NS::Application* ApplicationMain() { \
		return new _APP(); \
	}

int main() {
	USING_ENGINE;

	ApplicationEntryPoint ep{};
	try {
		Application* app = ApplicationMain();

		if (app != nullptr) {
			auto& engine = Engine::GetInstance();

#if ANDROMEDA_LINUX || ANDROMEDA_WIN || ANDROMEDA_MAC
			engine.SetGraphicsAPI(graphics::Renderer::API::Vulkan);
#endif

			engine.Run(app);
		}

		delete app;
	} catch (const std::exception& e) {
		error(e.what());
	}
	return 0;
}