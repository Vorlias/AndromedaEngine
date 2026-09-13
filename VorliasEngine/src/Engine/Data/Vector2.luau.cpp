#include "Engine/Data/Vector.h"
#include <math.h>
#include "lualib.h"
#include "Engine/Luau/AtomsDef.h"
#include "Engine/Luau/Lib.h"

constexpr const char* kVector2 = "Vector2";

andromeda::Vector2* luaL_newVector2(lua_State* L, float x, float y) {
	auto vec = andromeda_luau::newuserdatamt<andromeda::Vector2>(L, kVector2Tag);
	vec->x = x;
	vec->y = y;
    return vec;
}

void andromeda_luau::luaL_pushVector2(lua_State* L, const andromeda::Vector2& value) {
	luaL_newVector2(L, value.x, value.y);
}

andromeda::Vector2* andromeda_luau::luaL_toVector2(lua_State* L, int idx) {
	return touserdata<andromeda::Vector2>(L, 1, kVector2Tag);
}

static int Vector2_new(lua_State* L) {
	float x = static_cast<float>(luaL_optnumber(L, 1, 0));
	float y = static_cast<float>(luaL_optnumber(L, 2, 0));

    luaL_newVector2(L, x, y);
	return 1;
}

static int Vector2_magnitude(lua_State* L) {
	auto vec = andromeda_luau::touserdata<andromeda::Vector2>(L, 1, kVector2Tag);
    lua_pushnumber(L, vec->GetMagnitude());
    return 1;
}

static int Vector2_normalize(lua_State* L) {
	auto vec = andromeda_luau::touserdata<andromeda::Vector2>(L, 1);
	auto magnitude = vec->GetMagnitude();

    if (magnitude > kEpsilon) {
        luaL_newVector2(L, vec->x / magnitude, vec->y / magnitude);
    } else {
        luaL_newVector2(L, 0, 0);
    }
	
    return 1;
}

static int Vector2_index(lua_State* L) {
	int atom;
	const char* property_name = lua_tostringatom(L, 2, &atom);
	if (property_name == nullptr) {
		luaL_error(L, "invalid property type on Vector2: %s", luaL_typename(L, 2));
	}

	switch (atom) {
		case ATOM_MAGNITUDE:
			return Vector2_magnitude(L);
		case ATOM_NORMALIZED:
			return Vector2_normalize(L);
	}

	luaL_error(L, "%s is not a valid member of Vector3", property_name);
	return 0;
}

static int Vector2_newindex(lua_State* L) {
	int atom;
	const char* property_name = lua_tostringatom(L, 2, &atom);
	if (property_name == nullptr) {
		luaL_error(L, "invalid property type on Vector2: %s", luaL_typename(L, 2));
	}

	luaL_error(L, "%s is not a valid member of Vector2", property_name);
	return 0;
}

static int Vector2_namecall(lua_State* L) {
	int atom;
	const char* method_name = lua_namecallatom(L, &atom);

	luaL_error(L, "unknown method %s on Vector3", method_name);
	return 0;
}

static int Vector2_tostring(lua_State* L) {
	auto vec = andromeda_luau::touserdata<andromeda::Vector2>(L, 1);
	auto str = andromeda::to_string(*vec);

	lua_pushstring(L, str.data());
	return 1;
}

static const luaL_Reg vector2Lib[] = {
	{"new", Vector2_new},
	{nullptr, nullptr},
};

void andromeda::registerVector2Lib(lua_State* L) {
	int _Top = lua_gettop(L);

	luaL_newmetatable(L, kVector2);

	lua_pushcfunction(L, Vector2_namecall, "Vector2_namecall");
	lua_setfield(L, -2, "__namecall");

	lua_pushcfunction(L, Vector2_index, "Vector2_index");
	lua_setfield(L, -2, "__index");

	// lua_pushcfunction(L, Vector3_newindex, "Vector3_newindex");
	// lua_setfield(L, -2, "__newindex");

	lua_pushcfunction(L, Vector2_tostring, "Vector2_tostring");
	lua_setfield(L, -2, "__tostring");

	lua_pushliteral(L, "Vector2");
	lua_setfield(L, -2, "__type");



	lua_setreadonly(L, -1, true);
	// lua_setmetatable(L, -2);
	lua_setuserdatametatable(L, kVector2Tag);
	lua_pop(L, 1);

	luaL_register(L, kVector2, vector2Lib);

	{
		// lua_pushvector(L, 0, 0, 0);
		// lua_setfield(L, -2, "zero");

		// lua_pushvector(L, 1, 1, 1);
		// lua_setfield(L, -2, "one");

		// lua_pushvector(L, 0, -1, 0);
		// lua_setfield(L, -2, "down");

		// lua_pushvector(L, 0, 0, 1);
		// lua_setfield(L, -2, "forward");

		// lua_pushvector(L, -1, 0, 0);
		// lua_setfield(L, -2, "left");

		// lua_pushvector(L, 1, 0, 0);
		// lua_setfield(L, -2, "right");

		// lua_pushvector(L, 0, 1, 0);
		// lua_setfield(L, -2, "up");
	}

	// vector3 mt
	{
		lua_newtable(L);
		// lua_pushcfunction(L, Vector3Lib_tostring, "Vector3Lib_tostring");
		// lua_setfield(L, -2, "__tostring");
		lua_setreadonly(L, -1, true);
		lua_setmetatable(L, -2);
	}

	lua_setreadonly(L, -1, true);
	ANDROMEDA_ASSERT(lua_gettop(L) == _Top);
}