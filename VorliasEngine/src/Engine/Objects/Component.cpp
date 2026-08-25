#include "lualib.h"

#include "Engine/Objects/Component.h"
#include "Engine/Luau/ComponentUserdata.h"
#include "Engine/Luau/Userdata.h"
#include "Engine/Objects/Object.h"
#include "Engine/Luau/LuauScript.h"
#include "Engine/Luau/Lib.h"

andromeda::LuauComponentType andromeda::GetComponentTypeFromString(std::string_view str) {
	if (str == "Transform") {
		return LuauComponentType::Transform;
	} else if (str == "LuauScript") {
		return LuauComponentType::LuauScript;
	}

	return LuauComponentType::Null;
}

int andromeda::PushComponent(lua_State* L, andromeda_luau::EntityHandle* handle, andromeda::LuauComponentType componentType) {
	switch (componentType) {
		case andromeda::LuauComponentType::LuauScript: {
			andromeda_luau::LuauComponent<andromeda::LuauScriptComponent>::Push(L, *handle, *handle);
			return 1;
		}
		case andromeda::LuauComponentType::Transform: {
			andromeda_luau::LuauComponent<andromeda::TransformComponent>::Push(L, *handle, *handle);
			return 1;
		}
		default:
			lua_pushnil(L);
			return 1;
	}
}

static andromeda_luau::LuauUserdataType<andromeda::TransformComponent> s_transformComponentUserdata;
static andromeda_luau::LuauUserdataType<andromeda::LuauScriptComponent> s_scriptComponentUserdata;

int getScriptPath(lua_State* L, andromeda::LuauScriptComponent* component) {
	auto script = component->GetScript();
	lua_pushstring(L, script->GetFilePath().c_str());
	return 1;
}

void andromeda::RegisterComponents(lua_State* L) {
	using namespace andromeda_luau;

	if (!s_transformComponentUserdata) {
		s_transformComponentUserdata.SetName("Transform")
			.SetTag(kTransformComponent)
			.AddField("position", &TransformComponent::position)
			.AddField("rotation", &TransformComponent::rotation);
	}

	if (!s_scriptComponentUserdata) {
		s_scriptComponentUserdata.SetName("LuauScript")
			.SetTag(kLuauScriptComponent)
			.AddGetter("filePath", getScriptPath)
			.AddGetter("enabled", &LuauScriptComponent::GetEnabled)
			.AddSetter("enabled", &LuauScriptComponent::SetEnabled);
	}

	LuauComponent<TransformComponent>::RegisterType(L, &s_transformComponentUserdata);
	LuauComponent<LuauScriptComponent>::RegisterType(L, &s_scriptComponentUserdata);
}
