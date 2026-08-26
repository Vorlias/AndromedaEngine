#include "Engine/Scene/Scene.h"
#include "Engine/Objects/Object.h"
#include "Engine/Log.h"
#include "Engine/Objects/Component.h"
#include "Engine/Luau/LuauScript.h"
#include "Engine/Luau/Lib.h"
using namespace andromeda;

Scene::Scene() {
}

Entity Scene::CreateEntity(const std::string& name) {
	Entity entity = {this, m_registry.create()};

	auto& name_component = entity.AddComponent<NameComponent>();
	name_component.name = name;

	auto& relationship = entity.AddComponent<EntityRelationships>();

	auto& transform = entity.AddComponent<TransformComponent>();

	return entity;
}

Entity Scene::CreateEntity() {
	return CreateEntity("Entity");
}

void Scene::DestroyEntity(Entity entity) {
	m_registry.destroy(entity.GetHandle());
}

void Scene::Initialize() {
	Awake();
	// TODO: Iterate scripts, inject any referent properties
	Start();
}

void Scene::Shutdown() {
	m_active = false;
	auto scriptView = m_registry.view<LuauScriptComponent>();

	for (auto [entity, component] : scriptView.each()) {
		if (component.HasError() || component.m_script == nullptr)
			continue;

		component.Close();
	}
}

void Scene::Awake() {
	auto scriptView = m_registry.view<LuauScriptComponent>();

	for (auto [entity, component] : scriptView.each()) {
		// Awake a script if possible
		if (component.HasError() || component.m_script == nullptr)
			continue;

		if (component.GetState() == LuauScriptComponent::STATE_ASLEEP && component.IsEnabled()) {
			component.m_entity = Entity(this, entity);
			component.Awake();
		}
	}
}

void Scene::Start() {
	auto scriptView = m_registry.view<LuauScriptComponent>();

	for (auto [entity, component] : scriptView.each()) {
		// Awake a script if possible
		if (component.HasError() || component.m_script == nullptr)
			continue;

		if (component.GetState() == LuauScriptComponent::STATE_AWAKE && component.IsEnabled()) {
			component.Start();
		}
	}

	m_active = true;
}

void Scene::Update(float dt) {
	if (!m_active) {
		andromeda::warn("Scene '{}' is inactive", m_name);
		return;
	}

	auto scriptView = m_registry.view<LuauScriptComponent>();
	for (auto [entity, component] : scriptView.each()) {
		// Awake a script if possible
		if (component.HasError() || component.m_script == nullptr)
			continue;

		if (component.GetState() == LuauScriptComponent::STATE_ASLEEP && component.IsEnabled()) {
			component.Awake();
		}
	}

	auto scriptUpdate = m_registry.view<const LuauScriptComponent, LuauScriptComponent::UpdateLifecycle>();
	for (auto [_, component] : scriptUpdate.each()) {
		if (component.GetState() != LuauScriptComponent::STATE_CLOSED) {
			if (component.IsEnabled()) {
				component.Update(dt);
			}
		} else
			andromeda::warn("State is still part of update lifecycle but has closed");
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

Scene::~Scene() {
	if (m_active)
		Shutdown();
}