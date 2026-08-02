#include "LuauState.h"
#include <lualib.h>
#include <spdlog/spdlog.h>
#include <stacktrace>
#include <iostream>

using namespace ENGINE_NS;

static const luaL_Reg lua_globals[] = {
	// {"print", luaP_print},
	// {"warn", luaP_warn},
	// {"error", luaP_error},
	{nullptr, nullptr},
};



LuauState::LuauState(LuauStateContext context) {
	L = luaL_newstate();

	// Open libraries
	luaL_openlibs(L);
	spdlog::info("Open libs");

	// Protect core libraries and metatables from modification
	luaL_sandbox(L);

	// Create a new writable global table for current thread
	luaL_sandboxthread(L);

	// Add globals
	lua_pushvalue(L, LUA_GLOBALSINDEX);
	luaL_register(L, nullptr, lua_globals);
	lua_pop(L, 1);

	// self reference
	lua_pushlightuserdata(L, this);
	lua_rawsetfield(L, LUA_REGISTRYINDEX, kLuauState);
}

LuauState* LuauState::GetLuauState(lua_State* L) {
	if (L == nullptr) {
		return nullptr;
	}

	lua_rawgetfield(L, LUA_REGISTRYINDEX, kLuauState);
	LuauState* state = static_cast<LuauState*>(lua_tolightuserdata(L, -1));
	lua_pop(L, -1);
	return state;
}

lua_State* LuauState::GetLuaState() {
	return L;
}

LuauState::~LuauState() {
	lua_close(L);
}