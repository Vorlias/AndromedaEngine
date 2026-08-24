
// #define ANDROMEDA_INTERNAL 1
#define ANDROMEDA_OPENGL 1
#define ANDROMEDA_EDITOR 0
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

void Test() {}

int main() {
	using namespace andromeda_luau;

	lua_State* L = luaL_newstate();

	LuauUserdataBuilder<TestStruct> test("TestStruct");
	LuauUserdataType<TestStruct> ty = test.AddGetter("test", &TestStruct::GetMemberValue).Build();

	LuauComponent<TestStruct>::RegisterType(L, &ty);

	entt::registry reg;
	auto entity = reg.create();
	auto component = reg.emplace<TestStruct>(entity);

	auto value = LuauComponent<TestStruct>::Push(L, &reg, entity);
	std::cout << value << std::endl;

	lua_getfield(L, -1, "test");
	float value2 = lua_tonumber(L, -1);
	std::cout << value2 << std::endl;

	// LuauStack s(L);

	// TestStruct inst;
	// inst.member = 100;

	// LuauUserdataBuilder<TestStruct> test("TestStruct");
	// test.AddGetter("test", &TestStruct::GetMemberValue);
	// test.AddSetter("test", &TestStruct::SetMemberValue);
	// auto ud = test.Build();
	
	// LuauComponent<LuauScriptComponent> lsc;

	// LuauComponentBuilder<TestStruct> test(L, "TestStruct");
	// test.AddProperty("test", &TestStruct::member);
	// test.AddMethod("testMethod", &TestStruct::DoSomething);

	// auto method = test.FindMethod("testMethod");
	// std::cout << "method is " << method << std::endl;

	// LuauApplication* app = new LuauApplication();
	// auto& engine = Engine::GetInstance();
	// engine.Run(app);

	// VulkanApplication app;
	// if (app.Initialize()) {
	// 	app.Run();
	// }

	// app.Shutdown();
	return 0;
}