#include <lua.h>

lua_State* luaL_spawnthread(lua_State* L, int idx, int* argc);
int luaL_runthread(lua_State* L, lua_State* T, int narg);

// Closes the thread
void luaL_closethread(lua_State* L);

namespace andromeda_luau {
	void openTaskLib(lua_State* L);

	// Run the scheduler for the given luau state
	void runThreadScheduler(lua_State* L, float time);
} // namespace andromeda_luau