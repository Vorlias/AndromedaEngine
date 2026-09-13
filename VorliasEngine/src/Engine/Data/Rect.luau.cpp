#include "Engine/Data/Rect.h"
#include "Engine/Luau/AtomsDef.h"
#include "Engine/Luau/Lib.h"
#include "Engine/Data/Vector.h"

constexpr const char* kRect = "Rect";

andromeda::Rect* andromeda_luau::luaL_newRect(lua_State* L) {
	auto vec = andromeda_luau::newuserdatamt<andromeda::Rect>(L, kRectTag);
	return vec;
}

andromeda::Rect* andromeda_luau::luaL_pushRect(lua_State* L, const andromeda::Vector2& position, const andromeda::Vector2& size) {
	andromeda::Rect* rect = luaL_newRect(L);
	rect->size = size;
	rect->position = position;
	return rect;
}

andromeda::Rect* andromeda_luau::luaL_pushRect(lua_State* L, float left, float top, float width, float height) {
	andromeda::Rect* rect = luaL_newRect(L);
	rect->size = andromeda::Vector2(width, height);
	rect->position = andromeda::Vector2(left, top);
	return rect;
}

static int Rect_new(lua_State* L) {
	using namespace andromeda_luau;
	int top = lua_gettop(L);

	if (lua_isuserdata(L, 1)) {
		luaL_checkudata(L, 1, "Vector2");
		luaL_checkudata(L, 2, "Vector2");

		// assume position, size
		auto position = touserdata<andromeda::Vector2>(L, 1, kVector2Tag);
		auto size = touserdata<andromeda::Vector2>(L, 2, kVector2Tag);

		auto rect = luaL_newRect(L);
		if (position != nullptr) {
			rect->position = *position;
		}

		if (size != nullptr) {
			rect->size = *size;
		}
	} else {
		// assume x, y, w, h
		float x = luaL_optnumber(L, 1, 0);
		float y = luaL_optnumber(L, 2, 0);
		float w = luaL_optnumber(L, 3, 0);
		float h = luaL_optnumber(L, 4, 0);

		luaL_pushRect(L, x, y, w, h);
	}

	return 1;
}

static int Rect_index(lua_State* L) {
    using namespace andromeda_luau;

	auto vec = touserdata<andromeda::Rect>(L, 1, kRectTag);

	int atom;
	const char* property_name = lua_tostringatom(L, 2, &atom);
	if (property_name == nullptr) {
		luaL_error(L, "invalid property type on Vector2: %s", luaL_typename(L, 2));
	}

	switch (atom) {
		case ATOM_POSITION:
			luaL_pushVector2(L, vec->position);
			return 1;
		case ATOM_SIZE:
			luaL_pushVector2(L, vec->size);
			return 1;
        case ATOM_MIN:
            luaL_pushVector2(L, vec->GetMin());
            return 1;
        case ATOM_CENTER:
            luaL_pushVector2(L, vec->GetCenter());
            return 1;
        case ATOM_MAX:
            luaL_pushVector2(L, vec->GetMax());
            return 1;
        case ATOM_X:
            lua_pushnumber(L, vec->position.x);
            return 1;
        case ATOM_Y:
            lua_pushnumber(L, vec->position.y);
            return 1;
        case ATOM_WIDTH:
            lua_pushnumber(L, vec->size.x);
            return 1;
        case ATOM_HEIGHT:
            lua_pushnumber(L, vec->size.y);
            return 1;
	}

	luaL_error(L, "%s is not a valid member of Vector3", property_name);
	return 0;
}

static int Rect_tostring(lua_State* L) {
	auto vec = andromeda_luau::touserdata<andromeda::Rect>(L, 1);
	auto str = andromeda::to_string(*vec);

	lua_pushstring(L, str.data());
	return 1;
}

static const luaL_Reg rectLib[] = {
	{"new", Rect_new},
	{nullptr, nullptr},
};

void andromeda_luau::registerRectLib(lua_State* L) {
	int _Top = lua_gettop(L);

	luaL_newmetatable(L, kRect);

	lua_pushcfunction(L, Rect_tostring, "Rect_tostring");
	lua_setfield(L, -2, "__tostring");

	lua_pushcfunction(L, Rect_index, "Rect_index");
	lua_setfield(L, -2, "__index");

	lua_pushliteral(L, "Rect");
	lua_setfield(L, -2, "__type");

	lua_setreadonly(L, -1, true);
	lua_setuserdatametatable(L, kRectTag);
	lua_pop(L, 1);

	luaL_register(L, kRect, rectLib);

	lua_setreadonly(L, -1, true);
	ANDROMEDA_ASSERT(lua_gettop(L) == _Top);
}