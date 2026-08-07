#include <iostream>
#include <Engine/Memory.h>
#include "Engine/Main.h"
#include "Engine/Window.h"
#include "Engine/FileDialogs.h"
#include "Engine/IMGUI.h"


// #include <SDL3/SDL.h>

// #include "Engine/Platform.h"
// #include "Engine/Time.h"
// #include "Engine/DateTime.h"


class GameApplication : public andromeda::Application {
	const andromeda::WindowOptions GetWindowOptions() const override {
		return andromeda::WindowOptions("", andromeda::Vector2u(800, 600), andromeda::WindowFlags::Default);
	}

	void WindowEvent(SDL_Event& e) override {

	}

	void RawRender(andromeda::graphics::Renderer& r) override {
		
	}

	void Update(float deltaTime) override {
		// std::cout << " time is " << deltaTime << ", elapsed = " << GetElapsedTime() << std::endl;
	}

	bool Initialize() override {
		SetFramerateLimit(60);
		
		auto opts = andromeda::WindowOptions("Test", andromeda::Vector2u(800, 600), andromeda::WindowFlags::Default);
		CreateWindow(opts);

		return true;
	}
};

andromeda::Application* ApplicationMain(const andromeda::ApplicationInit& init) {
	return new GameApplication();
}