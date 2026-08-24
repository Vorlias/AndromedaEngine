#include <entt/entt.hpp>


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
			if (childCount == 0) return; // if no children, just skip this lol
		}
	};
} // namespace andromeda