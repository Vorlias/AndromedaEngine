#include <iostream>
#include "Engine/Main.h"
#include "Engine/Window.h"

class GameApplication : public andromeda::Application {
	bool Initialize() override {
		using namespace andromeda;
		return true;
	}

	void Update() override {
	}

	void Render() override {
	}

	void Shutdown() override {
		using namespace andromeda;
	}
};

andromeda::Application* ApplicationMain() {
	return new GameApplication();
}