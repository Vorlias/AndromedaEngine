#include "lua.h"
#include "lualib.h"

namespace andromeda_luau {
	void luaL_registerlibrary(lua_State* L, const char* libName, const luaL_Reg* lib, bool readonly);
    void luaL_registermetatable(lua_State* L, const char* objectName, const luaL_Reg* mt, uint32_t tag = LUA_UTAG_LIMIT);

    // void luaL_registerconstructor(lua_State* L, int idx, const char* objectName, const char* name, lua_CFunction ctor);
} // namespace andromeda_luau


// Start a stack check
#define ANDROMEDAL_BEGIN(L) int _StkBegin = lua_gettop(L);
#define ANDROMEDAL_END(L) ANDROMEDA_ASSERT(lua_gettop(L) == _StkBegin)

#define ANDROMEDAL_ASSERTLSTCKC(L, O) ANDROMEDA_ASSERT(lua_gettop(L) == O)
#define ANDROMEDAL_STACKSEC(L, EXPR) { int _Top = lua_gettop(L); EXPR; ANDROMEDA_ASSERT(lua_gettop(L) == _Top); }