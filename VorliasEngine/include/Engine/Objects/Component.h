#pragma once
#include <entt/entt.hpp>
#include "Engine/Data/Vector.h"
#include "Engine/LinkedList.h"
#include <list>

struct lua_State;

namespace andromeda_luau {
	struct EntityHandle;
}

namespace andromeda {

	struct EntityRelationships {
		uint32_t sortOrder = 0;

		// The parent of this entity
		entt::entity parent{entt::null};

		// The children of this entity
		LinkedList<entt::entity> children{};

		constexpr size_t size() const { return children.size(); }
	};
	
	struct EntitySort {
		int32_t level{};
		int32_t order{};

		EntitySort() = default;
		EntitySort(int32_t order): order(order) {}
	};

	enum struct LuauComponentType {
		Null = -1,
		Transform = 0,
		LuauScript = 1,
	};

	LuauComponentType GetComponentTypeFromString(std::string_view str);
	int PushComponent(lua_State* L, andromeda_luau::EntityHandle* handle, LuauComponentType componentType);

	void RegisterComponents(lua_State* L);

	struct NameComponent {
		std::string name;
		std::string tag;
	};

	struct TransformComponent {
		Vector3 position = {0.0f, 0.0f, 0.0f};
		Vector3 rotation = {0.0f, 0.0f, 0.0f};
		Vector3 scale = {1.0f, 1.0f, 1.0f};
	};

	struct CameraComponent {
		int _placeholder;
	};
} // namespace andromeda