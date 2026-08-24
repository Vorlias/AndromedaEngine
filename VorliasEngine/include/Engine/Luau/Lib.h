#pragma once
#include "lua.h"
#include "lualib.h"

constexpr int kVector2Tag = 127;
constexpr int kEntityTag = 126;
constexpr int kComponentTag = 125;

namespace andromeda_luau {
	void luaL_registerlibrary(lua_State* L, const char* libName, const luaL_Reg* lib, bool readonly);
	void luaL_registermetatable(lua_State* L, const char* objectName, const luaL_Reg* mt, uint32_t tag = LUA_UTAG_LIMIT);

	template<typename T>
	T* newuserdatamt(lua_State* L, int tag);

	template<typename T>
	T* newuserdata(lua_State* L);

	template<typename T>
	T* touserdata(lua_State* L, int idx);

	template<typename T>
	T* touserdata(lua_State* L, int idx, int tag);

	namespace meta {
		CONSTSTR index = "__index";
		CONSTSTR newindex = "__newindex";
		CONSTSTR type = "__type";
		CONSTSTR tostring = "__tostring";
		CONSTSTR equal = "__eq";
		CONSTSTR call = "__call";
		CONSTSTR negate = "__unm";
		CONSTSTR add = "__add";
		CONSTSTR mul = "__mul";
		CONSTSTR div = "__div";
		CONSTSTR idiv = "__idiv";
		CONSTSTR sub = "__sub";
		CONSTSTR modulus = "__mod";
		CONSTSTR power = "__pow";

		CONSTSTR lessThan = "__lt";
		CONSTSTR lessOrEqual = "__le";
		CONSTSTR concat = "__concat";
		CONSTSTR iterator = "__iter";
		CONSTSTR length = "__len";

		CONSTSTR mode = "__mode";
		CONSTSTR metatable = "__metatable";

		CONSTSTR namecall = "__namecall";
	} // namespace meta

} // namespace andromeda_luau

#define luaL_setmetamethod(L, fn, k, idx) \
	lua_pushcfunction(L, fn, #fn); \
	lua_setfield(L, (idx < 0 ? -1 + idx : idx), k)

#define luaL_setmetatype(L, idx, k) \
	lua_pushliteral(L, k); \
	lua_setfield(L, (idx < 0 ? -1 + idx : idx), "__type")


template<typename T>
T* andromeda_luau::newuserdatamt(lua_State* L, int tag) {
	return new (lua_newuserdatataggedwithmetatable(L, sizeof(T), tag)) T{};
}

template<typename T>
T* andromeda_luau::newuserdata(lua_State* L) {
	return new (lua_newuserdata(L, sizeof(T))) T{};
}

template<typename T>
T* andromeda_luau::touserdata(lua_State* L, int idx) {
	return static_cast<T*>(lua_touserdata(L, idx));
}

template<typename T>
T* andromeda_luau::touserdata(lua_State* L, int idx, int tag) {
	return static_cast<T*>(lua_touserdatatagged(L, idx, tag));
}

// Start a stack check
#define ANDROMEDAL_BEGIN(L) int _StkBegin = lua_gettop(L);
#define ANDROMEDAL_END(L) ANDROMEDA_ASSERT(lua_gettop(L) == _StkBegin)

#define ANDROMEDAL_ASSERTLSTCKC(L, O) ANDROMEDA_ASSERT(lua_gettop(L) == O)
#define ANDROMEDAL_STACKSEC(L, EXPR) \
	{ \
		int _Top = lua_gettop(L); \
		EXPR; \
		ANDROMEDA_ASSERT(lua_gettop(L) == _Top); \
	}

#define ANDROMEDA_LASSERT(L, cond) \
	if (!(cond)) { \
		luaL_errorL(L, "assertion failed: " #cond); \
	}
#define ANDROMEDA_LASSERTM(L, cond, ...) \
	if (!(cond)) { \
		luaL_errorL(L, "assertion failed: " #cond " - " __VA_ARGS__); \
	}