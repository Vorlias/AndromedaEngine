#pragma once
#include "lualib.h"
#include "Engine/Data/Vector.h"
#include <map>
#include <unordered_map>

template<class... T>
constexpr bool always_false = false;

namespace andromeda_luau {
	// Luau stack manipulation wrapper
	class LuauStack {
	public:
		LuauStack(lua_State* L) : L(L) {}

		[[nodiscard]] constexpr int GetTop() {
			return lua_gettop(L);
		}

		void Clear() {
			lua_settop(L, 0);
		}

		void SetTop(int top) {
			ANDROMEDA_ASSERT(top >= 0);
			lua_settop(L, top);
		}

		template<typename T>
			requires(!std::is_void_v<T>)
		const char* GetTypeName() {
			if constexpr (std::is_same_v<T, bool>) {
				return "boolean";
			} else if constexpr ((std::is_integral_v<T> || std::is_floating_point_v<T> || std::is_enum_v<T>)) {
				return "number";
			} else if constexpr ((std::is_same_v<T, std::string> || std::is_same_v<T, std::string_view> || std::is_same_v<T, const char*>)) {
				return "string";
			} else if constexpr (std::is_same_v<T, andromeda::Vector3>) {
				return "Vector3";
			} else if constexpr (std::is_same_v<T, andromeda::Vector2>) {
				return "Vector2";
			} else {
				static_assert(always_false<T>, "Missing TypeName implementation for type");
			}
		}

		// Push values to the stack
		template<typename T>
			requires(!std::is_void_v<T>)
		void PushValue(const T value) {
			ANDROMEDA_ASSERT(lua_checkstack(L, 1));

			if constexpr (std::is_same_v<T, bool>) {
				lua_pushboolean(L, value);
			} else if constexpr (std::is_integral_v<T>) {
				lua_pushinteger(L, value);
			} else if constexpr (std::is_floating_point_v<T>) {
				lua_pushnumber(L, value);
			} else if constexpr (std::is_same_v<T, const char*>) {
				lua_pushstring(L, value);
			} else if constexpr (std::is_same_v<T, std::string>) {
				lua_pushstring(L, value.c_str());
			} else if constexpr (std::is_same_v<T, std::string_view>) {
				lua_pushlstring(L, value.data(), value.size());
			} else if constexpr (std::is_same_v<T, andromeda::Vector3>) {
				lua_pushvector(L, value.x, value.y, value.z);
			} else if constexpr (std::is_same_v<T, andromeda::Vector2>) {
				luaL_pushVector2(L, value);
			} else if constexpr (std::is_null_pointer_v<T>) {
				lua_pushnil(L);
			} else if constexpr (std::is_enum_v<T>) {
				lua_pushinteger(L, static_cast<int>(value));
			} else {
				static_assert(always_false<T>, "Missing PushValue implementation for type");
			}
		}

		template<typename T>
			requires(!std::is_void_v<T> && !std::is_null_pointer_v<T>)
		T GetValue(int idx) {
			ANDROMEDA_ASSERT(IsType<T>(idx));

			if constexpr (std::is_same_v<T, bool>) {
				return lua_toboolean(L, idx);
			} else if constexpr (std::is_integral_v<T>) {
				return lua_tointeger(L, idx);
			} else if constexpr (std::is_floating_point_v<T>) {
				return lua_tonumber(L, idx);
			} else if constexpr (std::is_enum_v<T>) {
				return static_cast<T>(lua_tointeger(L, idx));
			} else if constexpr (std::is_same_v<T, const char*> || std::is_same_v<T, std::string>) {
				return lua_tostring(L, idx);
			} else if constexpr (std::is_same_v<T, std::string_view>) {
				size_t size;
				const char* str = lua_tolstring(L, idx, &size);
				return std::string_view(str, size);
			} else if constexpr (std::is_same_v<T, andromeda::Vector3>) {
				const float* v = lua_tovector(L, idx);
				return andromeda::Vector3(v[0], v[1], v[2]);
			} else if constexpr (std::is_same_v<T, andromeda::Vector2>) {
				const auto* v = luaL_toVector2(L, idx);
				return *v;
			} else {
				static_assert(always_false<T>, "Missing GetValue implementation for type");
			}
		}

		template<typename T>
			requires(!std::is_void_v<T>)
		bool IsType(int idx) {
			if constexpr (std::is_same_v<T, bool>) {
				return lua_type(L, -1) == LUA_TBOOLEAN;
			} else if constexpr (std::is_integral_v<T>) {
				return lua_type(L, -1) == LUA_TNUMBER;
			} else if constexpr (std::is_floating_point_v<T>) {
				return lua_type(L, -1) == LUA_TNUMBER;
			} else if constexpr (std::is_same_v<T, const char*>) {
				return lua_type(L, -1) == LUA_TSTRING;
			} else if constexpr (std::is_same_v<T, andromeda::Vector3>) {
				return lua_type(L, -1) == LUA_TVECTOR;
			} else if constexpr (std::is_null_pointer_v<T>) {
				return lua_type(L, -1) == LUA_TNIL;
			} else {
				static_assert(always_false<T>, "Missing IsType implementation for type");
			}
		}

		template<typename T>
		void PushArray(std::initializer_list<T> items) {
			lua_createtable(L, items.size(), 0);

			int i = 1;
			for (auto it = items.begin(); it != items.end(); it++) {
				PushValue(*it);
				lua_rawseti(L, -2, i);
				i++;
			}
		}

		template<typename T>
		void PushArray(const std::vector<T>& items) {
			lua_createtable(L, items.size(), 0);

			int i = 1;
			for (auto& item : items) {
				PushValue(item);
				lua_rawseti(L, -2, i);
				i++;
			}
		}

		template<typename T, std::size_t N>
		void PushArray(const std::array<T, N>& items) {
			lua_createtable(L, N, 0);

			for (int i = 0; i < N; i++) {
				PushValue(items[i]);
				lua_rawseti(L, -2, i + 1);
			}
		}

		template<typename T>
		void PushMap(const std::map<std::string, T>& values) {
			lua_createtable(L, 0, values.size());
			for (auto& [k, v] : values) {
				PushValue(v);
				lua_setfield(L, -2, k.c_str());
			}
		}

		template<typename K, typename V, typename _Hash, typename _Pred>
			requires(std::is_same_v<std::string, K> || std::is_same_v<const char*, K>)
		void PushMap(const std::unordered_map<K, V, _Hash, _Pred>& values) {
			lua_createtable(L, 0, values.size());
			for (auto& [k, v] : values) {
				PushValue(v);

				if constexpr (std::is_same_v<K, std::string>) {
					lua_setfield(L, -2, k.c_str());
				} else if constexpr (std::is_same_v<K, std::string_view>) {
					lua_setfield(L, -2, k.data());
				} else {
					lua_setfield(L, -2, k);
				}
			}
		}

		template<typename T>
		void SetField(int idx, const char* name, const T value) {
			PushValue(value);

			int ridx = idx < 0 ? -1 + idx : idx;
			luaL_checktype(L, ridx, LUA_TTABLE);
			lua_setfield(L, ridx, name);
		}

		template<typename T>
		T GetField(int idx, const char* name) {
			luaL_checktype(L, idx, LUA_TTABLE);
			lua_getfield(L, idx, name);
			return GetValue<T>(-1);
		}

		int Length(int idx) {
			return lua_objlen(L, idx);
		}

		// template<typename R, typename... Args>
		// R Call(int idx, Args const&... args) {
		// 	int argc = sizeof...(Args);
		// 	(PushValue(args), ...); // no way to do this in reverse AFAIK otherise this would be OP

		// 	if constexpr (std::is_void_v<R>) {
		// 		lua_call(L, argc, 0);
		// 		return;
		// 	} else {
		// 		lua_call(L, argc, 1);
		// 		return GetValue<R>(-1);
		// 	}
		// }

	private:
		lua_State* L;
	};
} // namespace andromeda_luau
