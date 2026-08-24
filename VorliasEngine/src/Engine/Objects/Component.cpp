#include "Engine/Objects/Component.h"
#include "lualib.h"
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

static andromeda_luau::LuauUserdataType<andromeda::TransformComponent> s_transformComponentUserdata;
static andromeda_luau::LuauUserdataType<andromeda::LuauScriptComponent> s_scriptComponentUserdata;

void andromeda::RegisterComponents(lua_State* L) {
	using namespace andromeda_luau;

	if (!s_transformComponentUserdata) {
		LuauUserdataBuilder<TransformComponent> transformComponent("Transform", kTransformComponent);
		s_transformComponentUserdata = transformComponent.Build();
	}

	if (!s_scriptComponentUserdata) {
		LuauUserdataBuilder<LuauScriptComponent> scriptComponent("LuauScript", kLuauScriptComponent);
		s_scriptComponentUserdata =
			scriptComponent.AddGetter("enabled", &LuauScriptComponent::GetEnabled).AddSetter("enabled", &LuauScriptComponent::SetEnabled).Build();
	}

	LuauComponent<TransformComponent>::RegisterType(L, &s_transformComponentUserdata);
	LuauComponent<LuauScriptComponent>::RegisterType(L, &s_scriptComponentUserdata);
}
