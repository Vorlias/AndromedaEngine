#include <iostream>
#include "Engine/Log.h"
#include "Engine/Window.h"

// class GameApplication : public andromeda::Application {
// 	bool Initialize() override {
// 		using namespace andromeda;
// 		LuauScript script;

// 		return true;
// 	}

// 	void Shutdown() override {
// 		using namespace andromeda;
// 		print("Shutting down!");
// 	}
// };

// MAIN_APP(GameApplication);

int main() {
	andromeda::Window window{};

	if (window.Initialize(andromeda::WindowOptions())) {
		while (!window.HasRequestedExit()) {
			window.PushEvents();
		}

		window.Shutdown();
	}

	return 0;
}