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

    // check
    template<> bool LuauStack::IsType<bool>(int idx) { return lua_type(L, -1) == LUA_TBOOLEAN; }
    template<> bool LuauStack::IsType<int>(int idx) { return lua_type(L, -1) == LUA_TNUMBER; }
    template<> bool LuauStack::IsType<float>(int idx) { return lua_type(L, -1) == LUA_TNUMBER; }
    template<> bool LuauStack::IsType<double>(int idx) { return lua_type(L, -1) == LUA_TNUMBER; }
    template<> bool LuauStack::IsType<const char*>(int idx) { return lua_type(L, -1) == LUA_TSTRING; }

	// push

	template<>
	void LuauStack::PushValue<bool>(bool value) {
		lua_pushboolean(L, value);
	}

	template<>
	void LuauStack::PushValue<float>(float value) {
		lua_pushnumber(L, value);
	}

	template<>
	void LuauStack::PushValue<double>(double value) {
		lua_pushnumber(L, value);
	}

	template<>
	void LuauStack::PushValue<const char*>(const char* value) {
		lua_pushstring(L, value);
	}

	template<>
	void LuauStack::PushValue<int>(int value) {
		lua_pushinteger(L, value);
	}

	// getters

	template<>
	bool LuauStack::GetValue<bool>(int idx) {
		return lua_toboolean(L, idx);
	}

	template<>
	float LuauStack::GetValue<float>(int idx) {
		return static_cast<float>(lua_tonumber(L, idx));
	}

	template<>
	double LuauStack::GetValue<double>(int idx) {
		return lua_tonumber(L, idx);
	}

	template<>
	const char* LuauStack::GetValue<const char*>(int idx) {
		return lua_tostring(L, idx);
	}

	template<>
	int LuauStack::GetValue<int>(int idx) {
		return lua_tointeger(L, idx);
	}
} // namespace andromeda_luau