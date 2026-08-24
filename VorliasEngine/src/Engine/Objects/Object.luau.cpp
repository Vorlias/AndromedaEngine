#include "Engine/Objects/Object.h"
#include "Engine/Objects/Component.h"
#include "Engine/Luau/Lib.h"

#include "lualib.h"

andromeda_luau::EntityHandle* andromeda_luau::pushEntityHandle(lua_State* L, andromeda::Scene* scene, entt::entity entity) {
	auto ent = andromeda_luau::newuserdatamt<andromeda_luau::EntityHandle>(L, kEntityTag);
	ent->entity = entity;
	ent->scene = scene;
	return ent;
}

andromeda_luau::EntityHandle* andromeda_luau::pushEntity(lua_State* L, const andromeda::Entity& entity) {
	if (entity) {
		return pushEntityHandle(L, entity.GetScene(), entity.GetHandle());
	} else {
		lua_pushnil(L);
		return nullptr;
	}
}

int Entity_index(lua_State* L) {
	auto entt = andromeda_luau::touserdata<andromeda_luau::EntityHandle>(L, 1);

	size_t property_name_len;
	int atom;
	const char* property_name = lua_tolstringatom(L, 2, &property_name_len, &atom);

	if (strcmp(property_name, "name") == 0) {
		andromeda::Entity entity = entt->GetEntity();
		std::string_view name = entity.GetName();

		lua_pushlstring(L, name.data(), name.size());
		return 1;
	}

	if (strcmp(property_name, "script") == 0) {
		// Not sure what do yet
		lua_pushnil(L);
		return 1;
	}

	if (strcmp(property_name, "id") == 0) {
		lua_pushinteger(L, (int) entt->entity);
		return 1;
	}

	if (strcmp(property_name, "parent") == 0) {
		andromeda::Entity ent = entt->GetEntity();
		auto parentEntity = ent.GetParent();
		andromeda_luau::pushEntity(L, parentEntity);
		return 1;
	}


	luaL_error(L, "Attempt to index entity with property '%s'", property_name);
	return 0;
}

int Entity_newindex(lua_State* L) {
	auto entt = andromeda_luau::touserdata<andromeda_luau::EntityHandle>(L, 1);

	size_t property_name_len;
	int atom;
	const char* property_name = lua_tolstringatom(L, 2, &property_name_len, &atom);

	if (strcmp(property_name, "name") == 0) {
		size_t len;
		const char* name = lua_tolstring(L, 3, &len);

		andromeda::Entity entity{entt->scene, entt->entity};
		entity.SetName(std::string(name, len));
		return 0;
	}

	luaL_error(L, "Attempt to index entity with property '%s'", property_name);
	return 0;
}

int Entity_namecall(lua_State* L) {
	auto entt = andromeda_luau::touserdata<andromeda_luau::EntityHandle>(L, 1);

	int atom;
	const char* method_name = lua_namecallatom(L, &atom);

	luaL_error(L, "Attempt to index entity with invalid method '%s'", method_name);
	return 0;
}

constexpr const char* kEntity = "Entity";
void andromeda_luau::registerObjectLib(lua_State* L) {
	int top = lua_gettop(L);
	{
		luaL_newmetatable(L, kEntity);

		lua_pushcfunction(L, Entity_index, "Entity_index");
		lua_setfield(L, -2, andromeda_luau::meta::index);

		lua_pushcfunction(L, Entity_newindex, "Entity_newindex");
		lua_setfield(L, -2, andromeda_luau::meta::newindex);

		luaL_setmetamethod(L, Entity_namecall, andromeda_luau::meta::namecall, -1);
		luaL_setmetatype(L, -1, "Entity");

		lua_setreadonly(L, -1, true);
		lua_setuserdatametatable(L, kEntityTag);
	}
	ANDROMEDA_ASSERT(lua_gettop(L) == top);
}