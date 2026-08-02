#include <iostream>
#include "Engine/Log.h"
// #define ANDROMEDA_EXPERIMENTAL 1
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
			SDL_Event e;
			while (window.PollSDLEvent(&e)) {
				switch (e.type) {
					case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
						window.Close();
						break;
				}
			}

			// while (const std::optional event = window.PollEvent()) {
			// 	if (event->is<andromeda::WindowEvent::Closed>()) {
			// 		window.Close();
			// 	} else if (event->is<andromeda::WindowEvent::WindowStateChange>()) {
			// 		auto stateChange = event->get<andromeda::WindowEvent::WindowStateChange>();
			// 		std::cout << "test" << std::endl;
			// 	}
			// }
		}

		andromeda::print("Shutting down?!");
		window.Shutdown();
	}

	return 0;
}