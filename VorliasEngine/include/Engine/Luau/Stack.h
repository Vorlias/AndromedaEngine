#pragma once
#include "lualib.h"

namespace andromeda_luau {
	class LuauStack {
	public:
		LuauStack(lua_State* L) : L(L) {}
		[[nodiscard]] constexpr int GetTop() {
			return lua_gettop(L);
		}

		template<typename T>
		void PushValue(const T value) {
			static_assert(false, "Overload for type not handled");
			ANDROMEDA_ASSERTM(false, "Type not implemented");
		}

		template<typename T>
		T GetValue(int idx) {
			static_assert(false, "Overload for type not handled");
			ANDROMEDA_ASSERTM(false, "Type not implemented");
		}

		template<typename T>
		bool IsType(int idx) {
			static_assert(false, "Overload for type not handled");
		}

	private:
		lua_State* L;
	};
} // namespace andromeda_luau


// check
template<>
inline bool andromeda_luau::LuauStack::IsType<bool>(int idx) {
	return lua_type(L, -1) == LUA_TBOOLEAN;
}
template<>
inline bool andromeda_luau::LuauStack::IsType<int>(int idx) {
	return lua_type(L, -1) == LUA_TNUMBER;
}
template<>
inline bool andromeda_luau::LuauStack::IsType<float>(int idx) {
	return lua_type(L, -1) == LUA_TNUMBER;
}
template<>
inline bool andromeda_luau::LuauStack::IsType<double>(int idx) {
	return lua_type(L, -1) == LUA_TNUMBER;
}
template<>
inline bool andromeda_luau::LuauStack::IsType<const char*>(int idx) {
	return lua_type(L, -1) == LUA_TSTRING;
}

// push

template<>
inline void andromeda_luau::LuauStack::PushValue<bool>(bool value) {
	lua_pushboolean(L, value);
}

template<>
inline void andromeda_luau::LuauStack::PushValue<float>(float value) {
	lua_pushnumber(L, value);
}

template<>
inline void andromeda_luau::LuauStack::PushValue<double>(double value) {
	lua_pushnumber(L, value);
}

template<>
inline void andromeda_luau::LuauStack::PushValue<const char*>(const char* value) {
	lua_pushstring(L, value);
}

template<>
inline void andromeda_luau::LuauStack::PushValue<int>(int value) {
	lua_pushinteger(L, value);
}

// getters

template<>
inline bool andromeda_luau::LuauStack::GetValue<bool>(int idx) {
	return lua_toboolean(L, idx);
}

template<>
inline float andromeda_luau::LuauStack::GetValue<float>(int idx) {
	return static_cast<float>(lua_tonumber(L, idx));
}

template<>
inline double andromeda_luau::LuauStack::GetValue<double>(int idx) {
	return lua_tonumber(L, idx);
}

template<>
inline const char* andromeda_luau::LuauStack::GetValue<const char*>(int idx) {
	return lua_tostring(L, idx);
}

template<>
inline int andromeda_luau::LuauStack::GetValue<int>(int idx) {
	return lua_tointeger(L, idx);
}