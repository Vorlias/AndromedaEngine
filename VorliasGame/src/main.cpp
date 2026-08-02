#include <iostream>
#include "Engine/Log.h"
#include "Engine/Main.h"
#include "Engine/Luau/LuauScript.h"

class GameApplication : public andromeda::Application {
	bool Initialize() override {
		using namespace andromeda;

		print("Hello, World!");

		LuauScript script;

		return true;
	}

	void Shutdown() override {
		using namespace andromeda;
		print("Shutting down!");
	}
};

MAIN_APP(GameApplication);