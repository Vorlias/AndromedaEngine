#include "Engine/Data/Vector.h"
#include <math.h>
#include "lualib.h"
#include "Engine/Luau/AtomsDef.h"


constexpr const char* kVector2 = "Vector2";

static const luaL_Reg vector2Lib[] = {
	{nullptr, nullptr},
};
void andromeda::registerVector2Lib(lua_State* L) {}


///////////////////////////////////////////////
//         VECTOR 3                         //
//////////////////////////////////////////////
constexpr const char* kVector3 = "Vector3";

static int Vector3_new(lua_State* L) {
	float x = static_cast<float>(luaL_optnumber(L, 1, 0));
	float y = static_cast<float>(luaL_optnumber(L, 2, 0));
	float z = static_cast<float>(luaL_optnumber(L, 3, 0));

	lua_pushvector(L, x, y, z);

	return 1;
}

static int Vector3_magnitude(lua_State* L) {
	const auto vec = reinterpret_cast<const andromeda::Vector3*>(luaL_checkvector(L, 1));
	lua_pushnumber(L, vec->GetMagnitude());
	return 1;
}

static int Vector3_index(lua_State* L) {
	int atom;
	const char* property_name = lua_tostringatom(L, 2, &atom);
	if (property_name == nullptr) {
		luaL_error(L, "invalid property type on Vector3: %s", luaL_typename(L, 2));
	}

	switch (atom) {
		case ATOM_MAGNITUDE:
			return Vector3_magnitude(L);
	}

	luaL_error(L, "%s is not a valid member of Vector3", property_name);
	return 0;
}

static int Vector3_newindex(lua_State* L) {
	int atom;
	const char* property_name = lua_tostringatom(L, 2, &atom);
	if (property_name == nullptr) {
		luaL_error(L, "invalid property type on Vector3: %s", luaL_typename(L, 2));
	}

	luaL_error(L, "%s is not a valid member of Vector3", property_name);
	return 0;
}

static int Vector3_namecall(lua_State* L) {
	int atom;
	const char* method_name = lua_namecallatom(L, &atom);

	luaL_error(L, "unknown method %s on Vector3", method_name);
	return 0;
}

static const luaL_Reg vector3Lib[] = {
	{"new", Vector3_new},
	{nullptr, nullptr},
};

void andromeda::registerVector3Lib(lua_State* L) {
	lua_pushvector(L, 0, 0, 0);
	luaL_newmetatable(L, kVector3);

	lua_pushcfunction(L, Vector3_namecall, "Vector3_namecall");
	lua_setfield(L, -2, "__namecall");

	lua_pushcfunction(L, Vector3_index, "Vector3_index");
	lua_setfield(L, -2, "__index");

	lua_pushliteral(L, "Vector3");
	lua_setfield(L, -2, "__type");

	lua_setreadonly(L, -1, true);
	lua_setmetatable(L, -2);
	lua_pop(L, 1);

	luaL_register(L, kVector3, vector3Lib);
}