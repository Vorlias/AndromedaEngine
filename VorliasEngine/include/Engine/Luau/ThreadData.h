#include <lua.h>

namespace andromeda_luau {
	struct LuauThreadData {
		// if thread is alive
		bool alive;

		// ref count
		int refCount;

		lua_State* ownThread;

		// main thread
		lua_State* mainThread;

		// script thread
		lua_State* scriptThread;
		int scriptRef;

		lua_State* workingThread;
		int workingRef;

		void CreateRef(lua_State* L);
	};

	LuauThreadData* createThreadData(lua_State* T);
	LuauThreadData* getThreadData(lua_State* T);
} // namespace andromeda_luau