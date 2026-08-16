#include "lua.h"

namespace andromeda_luau {
    void luaL_debugstack(lua_State* L);

    const char* luaL_toprettylstring(lua_State* L, int idx, size_t* len);
    void luaL_pushprettystring(lua_State* L, int idx);
}