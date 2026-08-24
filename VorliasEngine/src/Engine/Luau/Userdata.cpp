#include "Engine/Luau/Userdata.h"
#include <functional>

using namespace andromeda_luau;

// void Userdata::AddProperty(const char* name, lua_CFunction get, lua_CFunction set, int atom) {
// 	auto property = properties[name];
// 	property.name = name;
// 	property.getter = get;
// 	property.setter = set;
// 	property.atom = atom;
// }

// void Userdata::AddMethod(const char* name, lua_CFunction func, int atom) {
// 	UserdataMethod method{name, func, atom};
// 	methods.insert({name, method});

// 	if (atom > 0)
// 		methodAtoms.insert({atom, method});
// }

// static int __namecall(lua_State* L) {}
// static int __index(lua_State* L) {
// 	void* obj = lua_touserdata(L, 1); // obj calling
// }
// static int __newindex(lua_State* L) {}

// void Userdata::RegisterType(lua_State* L) {
// 	luaL_newmetatable(L, name);
// 	{
// 		if (properties.size() > 0) {
// 			lua_pushcfunction(L, __index, "__index");
// 			lua_setfield(L, -2, "__index");

// 			lua_pushcfunction(L, __newindex, "__newindex");
// 			lua_setfield(L, -2, "__newindex");
// 		}

// 		if (methods.size() > 0) {
// 			lua_pushcfunction(L, __namecall, "__namecall");
// 			lua_setfield(L, -2, "__namecall");
// 		}

// 		if (tostring != nullptr) {
// 			lua_pushcfunction(L, tostring, "tostring");
// 			lua_setfield(L, -2, "__tostring");
// 		}
// 	}

// 	lua_pushstring(L, name);
// 	lua_setfield(L, -2, "__type");

// 	lua_setreadonly(L, -1, true);
// 	if (tag < LUA_UTAG_LIMIT) {
// 		lua_setuserdatametatable(L, tag);
// 	} else {
// 		lua_pop(L, 1);
// 	}
// }