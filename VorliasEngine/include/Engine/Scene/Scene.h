#pragma once
#include <entt/entt.hpp>
namespace andromeda {
	class Entity;
	class Scene {
	public:
		Scene();

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

		// Initialize the scene
		void Initialize();

		// Update the scene
		void Update(float dt);
		
		// Shutdown the scene
		void Shutdown();

		~Scene();

		ANDROMEDA_GETCONST bool IsActive() { return m_active; }

		operator entt::registry*() {
			return &m_registry;
		}

	private:
		void Awake();
		void Start();

	private:
		bool m_active;
		std::string m_name = "Scene";
		entt::registry m_registry;

		friend class Entity;
	};


} // namespace andromeda
