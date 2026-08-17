#include <entt/entt.hpp>

namespace andromeda {
	struct EntityRelationship {
		// The parent of this entity
		entt::entity parent{entt::null};

		// The number of children on this entity
		std::size_t children{};

		// The entity identifier of the first child of this entity
		entt::entity first{entt::null};

		// The previous sibling of this entity
		entt::entity prev{entt::null};

		// The next sibling of this entity
		entt::entity next{entt::null};


		void AddChild(entt::registry& registry, entt::entity entity) {
			auto& child = first;
			if (first == entt::null) {
				first = entity;
				children = 1;
				std::cout << "updated child count to " << children << std::endl;
				return;
			}

			for (std::size_t i{}; i < children - 1; ++i) {
				child = registry.get<EntityRelationship>(child).next;
			}

			auto& lastChild = registry.get<EntityRelationship>(child);
			lastChild.next = entity;

			children += 1;
			std::cout << "updated child count to " << children << std::endl;
		}
	};
} // namespace andromeda