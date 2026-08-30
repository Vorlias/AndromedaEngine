#pragma once
#include <entt/entt.hpp>
#include "Engine/Data/Vector.h"

struct lua_State;

namespace andromeda_luau {
	struct EntityHandle;
}

namespace andromeda {

	struct EntityRelationships {
		// The parent of this entity
		entt::entity parent{entt::null};

		// The number of children on this entity
		std::size_t childCount{};

		// The entity identifier of the first child of this entity
		entt::entity firstChild{entt::null};

		// The previous sibling of this entity
		entt::entity prevSibling{entt::null};

		// The next sibling of this entity
		entt::entity nextSibling{entt::null};


		void AddChild(entt::registry& registry, entt::entity entity) {
			if (firstChild == entt::null) {
				firstChild = entity;
				childCount = 1;
				return;
			}

			auto& child = firstChild;
			for (std::size_t i{}; i < childCount - 1; ++i) {
				child = registry.get<EntityRelationships>(child).nextSibling;
			}

			auto& lastChild = registry.get<EntityRelationships>(child);
			lastChild.nextSibling = entity;

			childCount += 1;
		}

		void RemoveChild(entt::registry& registry, entt::entity entity) {
			if (childCount == 0)
				return; // if no children, just skip this lol

			childCount--;
		}
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