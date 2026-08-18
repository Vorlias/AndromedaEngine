#include "Engine/Objects/Object.h"
#include "Engine/Objects/Component.h"
#include "Engine/Luau/Lib.h"

#include "lualib.h"

andromeda::EntityHandle* andromeda::pushEntityHandle(lua_State* L, andromeda::Scene* scene, entt::entity entity) {
	auto ent = andromeda_luau::newuserdatamt<andromeda::EntityHandle>(L, kEntityTag);
	ent->entity = entity;
	ent->scene = scene;
	return ent;
}

constexpr const char* kEntity = "Entity";
void andromeda::registerObjectLib(lua_State* L) {
	int top = lua_gettop(L);
	{
		luaL_newmetatable(L, kEntity);

		lua_pushliteral(L, "Entity");
		lua_setfield(L, -2, "__type");

		lua_setreadonly(L, -1, true);
		lua_setuserdatametatable(L, kEntityTag);
		lua_pop(L, 1);
	}
	ANDROMEDA_ASSERT(lua_gettop(L) == top);
}