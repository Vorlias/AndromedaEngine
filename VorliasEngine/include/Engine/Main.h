#pragma once
#include "Application.h"
#include "Engine/Engine.h"
#include "Engine/Log.h"
#include <exception>
#include <iostream>

namespace andromeda {
	struct ApplicationInit {
		// The argumetns passed to this application
		const std::vector<std::string>& args;
		// The renderer to use for this application
		mutable graphics::Renderer::API renderer;
	};
} // namespace andromeda

andromeda::Application* ApplicationMain(const andromeda::ApplicationInit& init);

#define WINDOW_OPTIONS(...) \
	const andromeda::WindowOptions GetWindowOptions() const override { \
		return andromeda::WindowOptions(__VA_ARGS__); \
	}

// Define an application class as the main
#define ANDROMEDA_APP(_APP) \
	andromeda::Application* ApplicationMain(const andromeda::ApplicationInit& app) { \
		return new _APP(); \
	}

#define APP_MAIN(_AP) andromeda::Application* ApplicationMain(const andromeda::ApplicationInit& _AP)

int main(int argc, char* argv[]) {
	using namespace andromeda;

	std::vector<std::string> args;
	args.reserve(argc);

	for (int i = 1; i < argc; i++) {
		args.push_back(argv[i]);
	}

	ApplicationInit ep{
		.args = args,
		.renderer = graphics::Renderer::API::Vulkan,
	};

	try {
		Application* app = ApplicationMain(ep);

		if (app != nullptr) {
			auto& engine = Engine::GetInstance();
			engine.SetGraphicsAPI(ep.renderer);
			engine.Run(app);

			delete app;
		}
	} catch (const std::exception& e) {
		error(e.what());
	}
	return 0;
}