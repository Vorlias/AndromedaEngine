#pragma once
#include <entt/entt.hpp>
namespace andromeda {
	class Entity;
	class Scene {
	public:
		std::string_view GetName() const {
			return m_name;
		}

		void SetName(std::string_view name) {
			m_name = name;
		}

		constexpr entt::registry& GetRegistry() {
			return m_registry;
		}

		Entity CreateEntity(const std::string& name);
		Entity CreateEntity();

		template<typename T>
		void OnComponentAdded(Entity entity, T& component);

		void Update(float dt);

	private:
		std::string m_name = "Scene";
		entt::registry m_registry;

		friend class Entity;
	};


} // namespace andromeda

