#include "Engine/Luau/Lib.h"


void andromeda_luau::luaL_registerlibrary(lua_State* L, const char* libName, const luaL_Reg* lib, bool readonly) {
    lua_newtable(L);

    for (; lib->name; lib++) {
        lua_pushcfunction(L, lib->func, lib->name);
        lua_rawsetfield(L, -2, lib->name);
    }

    lua_newtable(L); // mt
    lua_setreadonly(L, -1, true);
    lua_setmetatable(L, -2);

    lua_setreadonly(L, -1, readonly);
    lua_pushvalue(L, -1);
    lua_setglobal(L, libName);
}