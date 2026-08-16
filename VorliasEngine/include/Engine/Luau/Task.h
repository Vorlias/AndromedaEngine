#include <lua.h>

lua_State* luaL_spawnthread(lua_State* L, int idx, int* argc);


namespace andromeda_luau {
	void openTaskLib(lua_State* L);

	// Run the scheduler for the given luau state
	void runThreadScheduler(lua_State* L, float time);
} // namespace andromeda_luau