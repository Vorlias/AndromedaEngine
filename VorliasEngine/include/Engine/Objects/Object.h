#pragma once
#include "Engine/Memory.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Data/Vector.h"
#include "entt/entt.hpp"
#include "Engine/Log.h"


using namespace entt::literals;

namespace andromeda {
	// class Scene;
	class Object {
	public:
		virtual std::string_view GetName() const = 0;
		virtual void SetName(std::string_view name) = 0;
	};

	struct Component {};

	struct NameComponent {
		std::string name;
	};

	struct TransformComponent {
		Vector3 position = {0.0f, 0.0f, 0.0f};
		Vector3 rotation = {0.0f, 0.0f, 0.0f};
		Vector3 scale = {1.0f, 1.0f, 1.0f};
	};

	class Entity : public Object {
	public:
		Entity() {};
		Entity(Scene* scene, entt::entity entity) : m_scene(scene), m_entity(entity) {}
		Entity(const Entity& other) = default;

		// Adds a tag component to this entity
		template<typename T>
			requires(std::is_empty<T>::value && std::is_default_constructible<T>::value)
		void AddComponent() {
			return m_scene->m_registry.emplace<T>(m_entity);
		}

		template<typename T>
		bool HasComponent() {
			return m_scene->m_registry.any_of<T>(m_entity);
		}

		template<typename T>
		void RemoveComponent() {
			m_scene->m_registry.remove<T>(m_entity);
		}

		// // Adds a tag component to this entity
		// template<typename T, typename... Args>
		// 	requires(std::is_empty<T>::value)
		// decltype(auto) AddComponent(Args&&... args) {
		// 	return m_scene->m_registry.emplace<T>(m_entity, std::forward<Args>(args)...);
		// }

		// Adds a value component to this entity
		template<typename T, typename... Args>
			requires(!std::is_empty<T>::value)
		T& AddComponent(Args&&... args) {
			T& component = m_scene->m_registry.emplace<T>(m_entity, std::forward<Args>(args)...);
			m_scene->OnComponentAdded<T>(*this, component);
			return component;
		}

		template<typename T>
		T& GetComponent() const {
			return m_scene->m_registry.get<T>(m_entity);
		}

		void SetParent(const Entity& parent);
		const std::vector<Entity> GetChildren() const;

		operator bool() const {
			return m_entity != entt::null;
		}
		operator entt::entity() const {
			return m_entity;
		}
		operator uint32_t() const {
			return (uint32_t)m_entity;
		}

		void SetName(std::string_view name) {
			auto& nc = GetComponent<NameComponent>();
			nc.name = name;
		}

		std::string_view GetName() const {
			auto& nc = GetComponent<NameComponent>();
			return nc.name;
		}

		Entity GetParent() const;

		ANDROMEDA_GETCONST Scene* GetScene() const { return m_scene; }
		ANDROMEDA_GETCONST entt::entity GetHandle() const { return m_entity; }
	private:
		friend class LuauScriptComponent;

		Scene* m_scene;
		entt::entity m_entity{entt::null};
	};
} // namespace andromeda

namespace andromeda_luau {
	struct EntityHandle {
		andromeda::Scene* scene;
		entt::entity entity{entt::null};

		andromeda::Entity GetEntity() const { 
			return andromeda::Entity{ scene, entity };
		}

		operator andromeda::Entity() {
			return andromeda::Entity{ scene, entity };
		}
	};

	struct ComponentHandle {
		void* component;
		enum {
			TYPE_TRANSFORM,
			TYPE_LUAUSCRIPT,
		} type;
	};


	void registerObjectLib(lua_State* L);

	EntityHandle* pushEntityHandle(lua_State* L, andromeda::Scene* scene, entt::entity entity);
	EntityHandle* pushEntity(lua_State* L, const andromeda::Entity& entity);

	template<typename T>
	T* pushComponent(lua_State* L, andromeda::Scene* scene, entt::entity entity);
} // namespace Luau

template<typename T>
void andromeda::Scene::OnComponentAdded(Entity entity, T& component) {} // annoyingly has to be here ?

#define TAG_COMPONENT(_Ty) \
	struct _Ty final {};