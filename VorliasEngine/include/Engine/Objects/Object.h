#pragma once
#include "Engine/Memory.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Data/Vector.h"
#include "entt/entt.hpp"
#include "Engine/Log.h"

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

		template<typename T, typename... Args>
		T& AddComponent(Args&&... args) {
			T& component = m_scene->m_registry.emplace<T>(m_entity, std::forward<Args>(args)...);
			m_scene->OnComponentAdded<T>(*this, component);
			return component;
		}

		template<typename T>
		T& GetComponent() {
			return m_scene->m_registry.get<T>(m_entity);
		}

		operator bool() const {
			return m_entity != entt::null;
		}
		operator entt::entity() const {
			return m_entity;
		}
		operator uint32_t() const {
			return (uint32_t)m_entity;
		}

		void SetName(std::string_view name) {}
		std::string_view GetName() const {
			return "";
		}

	private:
		Scene* m_scene;
		entt::entity m_entity{entt::null};
	};
} // namespace andromeda

template<typename T>
void andromeda::Scene::OnComponentAdded(Entity entity, T& component) {} // annoyingly has to be here ?