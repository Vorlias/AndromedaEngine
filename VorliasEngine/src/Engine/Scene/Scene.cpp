#include "Engine/Scene/Scene.h"

andromeda::Entity andromeda::Scene::CreateEntity(const std::string& name) {
	Entity entity = {this, m_registry.create()};

    auto name_component = entity.AddComponent<NameComponent>();
    name_component.name = name;

	return entity;
}

andromeda::Entity andromeda::Scene::CreateEntity() {
    return CreateEntity("Entity");
}