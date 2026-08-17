#include "Engine/Scene/Scene.h"
#include "Engine/Objects/Object.h"
#include "Engine/Log.h"
#include "Engine/Objects/Component.h"
#include "Engine/Luau/LuauScript.h"
using namespace andromeda;

Entity Scene::CreateEntity(const std::string& name) {
	Entity entity = {this, m_registry.create()};

	auto& name_component = entity.AddComponent<NameComponent>();
	name_component.name = name;

	auto& relationship = entity.AddComponent<EntityRelationship>();
	return entity;
}

Entity Scene::CreateEntity() {
	return CreateEntity("Entity");
}

void Scene::Update(float dt) {
	auto scriptView = m_registry.view<LuauScriptComponent>();
	for (auto [entity, component] : scriptView.each()) {
		// Awake a script if possible
		if (!component.IsAwake() && !component.HasError() && component.m_script != nullptr) {
			component.Awake();
		}
	}
}

template<>
void Scene::OnComponentAdded<TransformComponent>(Entity entity, TransformComponent& component) {}

template<>
void Scene::OnComponentAdded<NameComponent>(Entity entity, NameComponent& component) {
	andromeda::print("Transform added to {}", entity.GetName());
}

template<>
void Scene::OnComponentAdded<LuauScriptComponent>(Entity entity, LuauScriptComponent& component) {
	component.m_entity = entity;
}