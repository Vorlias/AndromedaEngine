#include <iostream>
#include <Engine/Memory.h>
#include "Engine/Main.h"
#include "Engine/Window.h"
#include "Engine/FileDialogs.h"

#include <SDL3/SDL.h>

// #include "Engine/Platform.h"
// #include "Engine/Time.h"
// #include "Engine/DateTime.h"


class GameApplication : public andromeda::Application {
	const andromeda::WindowOptions GetWindowOptions() const override {
		return andromeda::WindowOptions("", andromeda::Vector2u(800, 600), andromeda::WindowFlags::Default);
	}

	void Update() override {
		std::cout << " time is " << GetDeltaTime() << ", elapsed = " << GetElapsedTime() << std::endl;
	}

	bool Initialize() override {
		SetFramerate(200);
		return true;
	}
};

andromeda::Application* ApplicationMain(const andromeda::ApplicationInit& init) {
	return new GameApplication();
}