#pragma once
#include "lua.h"
#include "lualib.h"
#include "Lib.h"

namespace andromeda_luau {
	struct LuauMethod {
		LuauMethod* next = nullptr;

		const char* name;
		int atom = -1;
		lua_CFunction func{};
	};

	struct LuauMetamethod {
		LuauMetamethod* next;
		const char* name;
		lua_CFunction func;
	};

	template<typename T>
	struct LuauUserdataHandle {
		T* value;
	};

	template<typename T>
	struct LuauUserdataType {
		LuauUserdataType(const char* name) : name(name) {}
		LuauUserdataType(const char* name, int tag) : name(name), tag(tag) {
			ANDROMEDA_ASSERT(tag >= 0 && tag < LUA_UTAG_LIMIT);
		}

		typedef T value_type;
		const char* name = typeid(T).name(); // the name of the component type
		int tag{LUA_UTAG_LIMIT};

		static LuauMethod* methods = nullptr;
		LuauMetamethod* metaMethods = nullptr;

		LuauUserdataType<T>& AddMetaMethod(const char* name, lua_CFunction func) {
			auto next = metaMethods;

			auto metaMethod = new LuauMetamethod();
			metaMethod->name = name;
			metaMethod->func = func;
			metaMethod->next = next;

			metaMethods = metaMethod;
			return *this;
		}

		static LuauUserdataType<T>& AddMethod(const char* name, lua_CFunction func, int atom = -INT_MIN) {
			auto next = methods;

			auto method = new LuauMethod();
			method->name = name;
			method->func = func;
			method->atom = atom;
			method->next = next;

			methods = method;
			return *this;
		}

		static int Ud_index(lua_State* L) {
			size_t property_name_len;
			int atom;
			const char* property_name = lua_tolstringatom(L, 2, &property_name_len, &atom);

			luaL_error(L, "Attempt to index entity with property '%s'", property_name);
			return 0;
		}

		static int Ud_newindex(lua_State* L) {
			size_t property_name_len;
			int atom;
			const char* property_name = lua_tolstringatom(L, 2, &property_name_len, &atom);

			luaL_error(L, "Attempt to index entity with property '%s'", property_name);
			return 0;
		}

		static int Ud_namecall(lua_State* L) {
			int atom;
			const char* method_name = lua_namecallatom(L, &atom);

			for (auto method = methods; method; method = method->next) {
				if (method->atom == atom || strcmp(method->name, method_name) == 0) {
					return method->func(L);
				}
			}

			luaL_error(L, "Attempt to index entity with invalid method '%s'", method_name);
			return 0;
		}

		void RegisterType(lua_State* L) {
			luaL_newmetatable(L, name);

			for (auto metaMethod = metaMethods; metaMethod; metaMethod = metaMethod->next) {
				lua_pushcfunction(L, metaMethod->func, metaMethod->name);
				lua_setfield(L, -2, metaMethod->name);
			}

			lua_pushcfunction(L, Ud_namecall, "Ud_namecall");
			lua_setfield(L, -2, "__namecall");

			lua_pushstring(L, name);
			lua_setfield(L, -2, "__type");

			lua_setreadonly(L, -1, true);

			if (tag < LUA_UTAG_LIMIT) {
				lua_setuserdatametatable(L, tag);
			} else {
				lua_pop(L, 1);
			}
		}

		LuauUserdataHandle<T>* Push(lua_State* L) {
			LuauUserdataHandle<T>* handle = nullptr;
			if (tag < LUA_UTAG_LIMIT) {
				handle = newuserdatamt<LuauUserdataHandle<T>>(L, tag);
			} else {
				handle = newuserdata<LuauUserdataHandle<T>>(L);
				luaL_getmetatable(L, name);
				lua_setmetatable(L, -2);
			}

			return handle;
		}

		LuauUserdataHandle<T>* ToHandle(lua_State* L, int idx) {
			if (tag < LUA_UTAG_LIMIT) {
				return touserdata<LuauUserdataHandle<T>>(L, idx, tag);
			} else {
				return touserdata<LuauUserdataHandle<T>>(L, idx);
			}
		}
	};
} // namespace andromeda_luau

#define __UDNAME userdata

#define LUAU_BEGIN_USERDATA(_Ty, ...) \
	struct LuauUserdata : andromeda_luau::LuauUserdataType<_Ty> { \
		LuauUserdata() : LuauUserdataType(__VA_ARGS__) {}

#define LUAU_END_USERDATA() \
	} \
	static __UDNAME;
#define LUAU_IMPL_USERDATA(_Ty) _Ty::LuauUserdata _Ty::__UDNAME = _Ty::LuauUserdata()