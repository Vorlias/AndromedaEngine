#include <lua.h>

lua_State* luaL_spawnthread(lua_State* L, int idx, int* argc);
int luaL_runthread(lua_State* L, lua_State* T, int argc);


namespace andromeda_luau {
	void luaL_openTaskLib(lua_State* L);

	// Run the scheduler for the given luau state
	int luaL_runscheduler(lua_State* L, float time);
} // namespace andromeda_luau