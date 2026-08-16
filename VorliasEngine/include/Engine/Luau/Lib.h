#include "lua.h"
#include "lualib.h"

namespace andromeda_luau {
    void luaL_registerlibrary(lua_State* L, const char* libName, const luaL_Reg* lib, bool readonly);
}