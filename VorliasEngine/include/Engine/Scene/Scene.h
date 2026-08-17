#pragma once
#include <entt/entt.hpp>
#include "Engine/Objects/Object.h"

namespace andromeda {
	class Scene : public Object {
	public:
		std::string_view GetName() const override {
			return m_name;
		}

		void SetName(std::string_view name) override {
			m_name = name;
		}

		constexpr entt::registry& GetRegistry() {
			return m_registry;
		}

		Entity CreateEntity(const std::string& name);
        Entity CreateEntity();
	private:
		std::string m_name = "Scene";
		entt::registry m_registry;
		friend class Entity;
	};
} // namespace andromeda