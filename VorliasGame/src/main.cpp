
// #define ANDROMEDA_INTERNAL 1
#define ANDROMEDA_OPENGL 1
#define ANDROMEDA_EDITOR 0
#define DEBUG_LUAU_THREADS 1
// #define ANDROMEDA_MEMORY_DEBUG 1

#include "SDL3/SDL.h"

#include "Engine/Engine.h"
#include "Engine/Graphics/Vulkan/VulkanInstance.h"
#include "Engine/Graphics/Vulkan/VulkanRendererAPI.h"

#include "Engine/Window.h"
using namespace andromeda;

#include "VulkanPlayground.h"
#include "LuauTesting.h"

#include "Engine/Luau/Userdata.h"
#include "Engine/Luau/ComponentUserdata.h"
#include "Engine/Perf.h"
#include "Engine/Luau/Task.h"
#include "Engine/Luau/PrettyPrint.h"


struct TestStruct {
	int member;

	int DoSomething(std::string arg1, int arg2) {
		return 1;
	}

	void SetMemberValue(int value) {
		std::cout << "value is " << value << std::endl;
		member = value;
	}

	int GetMemberValue() const {
		return 1'000'000;
	}
};
 
int Test(lua_State* L) {
	std::cout << "got " << lua_gettop(L) << " args" << std::endl;

	andromeda_luau::luaL_debugstack(L);
	return 1;
}

int main() {
	using namespace andromeda_luau;

	lua_State* L = luaL_newstate();
	LuauStack stack(L);
	stack.PushArray({ 10, 20, 30 });
	stack.Clear();

	LuauApplication* app = new LuauApplication();
	auto& engine = Engine::GetInstance();
	engine.Run(app);

	// VulkanApplication app;
	// if (app.Initialize()) {
	// 	app.Run();
	// }

	// app.Shutdown();
	return 0;
}