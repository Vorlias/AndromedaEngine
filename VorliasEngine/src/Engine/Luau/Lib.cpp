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

void andromeda_luau::luaL_registermetatable(lua_State* L, const char* objectName, const luaL_Reg* metatable, uint32_t tag) {
	luaL_getmetatable(L, objectName);
	ANDROMEDA_ASSERT(lua_isnil(L, -1)); // ensure not already declared
	lua_pop(L, 1);

	luaL_newmetatable(L, objectName);
	luaL_register(L, nullptr, metatable);

	lua_pushstring(L, objectName);
	lua_setfield(L, -2, "__type");

	if (tag < LUA_UTAG_LIMIT)
		lua_setuserdatametatable(L, tag); 
}

void luaL_registerconstructor(lua_State* L, int idx, const char* objectName, const char* name, lua_CFunction ctor) {
	ANDROMEDAL_BEGIN(L);
	{
		luaL_getmetatable(L, objectName);

		if (lua_isnil(L, -1)) {
			// if not exists
			lua_pop(L, 1); // pop mt
			ANDROMEDAL_END(L);
			ANDROMEDA_ASSERTM(false, "Registering constructor with no metatable");
			return;
		}
	}
	ANDROMEDAL_END(L);
}