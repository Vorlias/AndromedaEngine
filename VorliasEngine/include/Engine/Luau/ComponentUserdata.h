#pragma once
#include "entt/entt.hpp"
#include "Lib.h"
#include "lualib.h"
#include "Userdata.h"

CONSTSTR kUserdata = "__userdata";
CONSTSTR kComponentTypes = "__COMPONENT";

namespace andromeda_luau {
	enum class LuauComponentType {
		Null = -1,
		LuauScript = 0,
		Transform = 1,
	};

	template<typename T>
	struct LuauComponent {
		entt::registry* registry;
		entt::entity entity;

		static int LuauComponentIndex(lua_State* L) {
			LuauComponent<T>* userdata = touserdata<LuauComponent<T>>(L, 1);
			LuauUserdataType<T>* userdataType = userdata->typedata;
			ANDROMEDA_ASSERT(userdataType != nullptr);

			size_t property_name_len;
			int atom;
			const char* property_name = lua_tolstringatom(L, 2, &property_name_len, &atom);
			LuauProperty<T>* prop = userdataType->FindProperty(property_name);
			if (prop == nullptr) {
				luaL_errorL(L, "Attempt to index %s with invalid property '%s'", userdataType->GetName(), property_name);
				return 0;
			}

			if (prop->get != nullptr) {
				T* component = userdata->registry->try_get<T>(userdata->entity);
				if (component == nullptr) {
					luaL_error(L, "%s was destroyed", userdataType->GetName());
					return 0;
				}

				int ret = prop->get(L, component);
				if (ret >= 0)
					return ret;
			}

			luaL_errorL(L, "%s.%s is not a readable property", userdataType->GetName(), property_name);
			return 0;
		};

		static int LuauComponentNewIndex(lua_State* L) {
			LuauComponent<T>* userdata = touserdata<LuauComponent<T>>(L, 1);
			LuauUserdataType<T>* userdataType = userdata->typedata;
			ANDROMEDA_ASSERT(userdataType != nullptr);

			size_t property_name_len;
			int atom;
			const char* property_name = lua_tolstringatom(L, 2, &property_name_len, &atom);
			LuauProperty<T>* prop = userdataType->FindProperty(property_name);
			if (prop == nullptr) {
				return 0;
			}

			if (prop->set != nullptr) {
				T* component = userdata->registry->try_get<T>(userdata->entity);
				if (component == nullptr) {
					luaL_error(L, "%s was destroyed", userdataType->GetName());
					return 0;
				}

				int ret = prop->set(L, component);
				if (ret >= 0)
					return ret;
			}

			luaL_errorL(L, "%s.%s is not a writable property", userdataType->GetName(), property_name);
			return 0;
		}

		static int LuauComponentNamecall(lua_State* L) {
			LuauComponent<T>* userdata = touserdata<LuauComponent<T>>(L, 1);
			LuauUserdataType<T>* userdataType = userdata->typedata;
			ANDROMEDA_ASSERT(userdataType != nullptr);

			int atom;
			const char* method_name = lua_namecallatom(L, &atom);

			LuauMethod<T>* method = userdataType->FindMethod(method_name);
			if (method != nullptr) {
				T* component = userdata->registry->try_get<T>(userdata->entity);
				if (component == nullptr) {
					luaL_error(L, "%s was destroyed", userdataType->GetName());
					return 0;
				}

				return method->invoke(L, component);
			}

			luaL_errorL(L, "%s is not a method of %s", method_name, userdataType->GetName());
			return 0;
		}

		static void RegisterType(lua_State* L, LuauUserdataType<T>* userdata) {
			const char* id = typeid(T).name();

			auto name = userdata->GetName();
			auto tag = userdata->GetTag();
			auto& methods = userdata->GetMethods();
			auto& properties = userdata->GetProperties();

			luaL_newmetatable(L, name);

			// insert into our registry
			lua_rawgetfield(L, LUA_REGISTRYINDEX, kComponentTypes); // + 1
			if (lua_isnil(L, -1)) {
				lua_pop(L, 1); // - 1

				lua_newtable(L); // + 1
				lua_pushvalue(L, -1); // + 1
				lua_rawsetfield(L, LUA_REGISTRYINDEX, kComponentTypes); // - 1
			}

			lua_pushlightuserdata(L, userdata); // + 1
			lua_setfield(L, -2, id); // - 1
			lua_pop(L, 1);

			if (properties.size() > 0) {
				lua_pushcfunction(L, LuauComponentIndex, "LuauComponentIndex");
				lua_setfield(L, -2, "__index");

				lua_pushcfunction(L, LuauComponentNewIndex, "LuauComponentNewIndex");
				lua_setfield(L, -2, "__newindex");
			}

			if (methods.size() > 0) {
				lua_pushcfunction(L, LuauComponentNamecall, "LuauComponentNamecall");
				lua_setfield(L, -2, "__namecall");
			}

			lua_pushstring(L, name);
			lua_setfield(L, -2, "__type");

			lua_setreadonly(L, -1, true);

			if (tag > 0 && tag < LUA_UTAG_LIMIT) {
				lua_setuserdatametatable(L, tag);
			} else {
				lua_pop(L, 1);
			}
		}

		static LuauComponent<T>* Push(lua_State* L, entt::registry* registry, entt::entity entity) {
			if (registry->try_get<T>(entity) == nullptr) {
				lua_pushnil(L);
				return nullptr;
			}

			lua_getfield(L, LUA_REGISTRYINDEX, kComponentTypes);
			if (lua_isnil(L, -1)) {
				luaL_error(L, "Component registry does not exist");
				return nullptr;
			}

			lua_getfield(L, -1, typeid(T).name());
			if (lua_isnil(L, -1)) {
				lua_pop(L, 2);
				luaL_error(L, "Type %s was not registered", typeid(T).name());
				return nullptr;
			}

			LuauUserdataType<T>* typedata = static_cast<LuauUserdataType<T>*>(lua_touserdata(L, -1));
			lua_pop(L, 2);

			if (typedata == nullptr) {
				luaL_error(L, "Typedata of %s is missing", typeid(T).name());
				return nullptr;
			}

			int tag = typedata->GetTag();
			LuauComponent<T>* ud;

			if (tag > 0) {
				ud = newuserdatamt<LuauComponent<T>>(L, tag);
			} else {
				ud = newuserdata<LuauComponent<T>>(L);
				luaL_getmetatable(L, typedata->GetName());
				lua_setmetatable(L, -2);
			}

			ud->entity = entity;
			ud->registry = registry;
			ud->typedata = typedata;
			return ud;
		}

	private:
		LuauUserdataType<T>* typedata;
	};

} // namespace andromeda_luau