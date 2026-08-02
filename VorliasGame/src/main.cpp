#include <iostream>
#include "Engine/Log.h"
#include "Engine/Main.h"
// #define ANDROMEDA_EXPERIMENTAL 1
#include "Engine/Window.h"

class GameApplication : public andromeda::Application {
	bool Initialize() override {
		using namespace andromeda;
		return true;
	}

	virtual andromeda::graphics::Renderer::API GetGraphicsAPI() const override {
		return andromeda::graphics::Renderer::API::Vulkan;
	}

	void Update() override {
		// andromeda::print("update frame");
	}

	void Render() override {
		// andromeda::print("render frame");
	}

	void Shutdown() override {
		using namespace andromeda;
		print("Shutting down!");
	}
};

MAIN_APP(GameApplication);
