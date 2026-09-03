#include "Engine/Scene/Scene.h"
#include "Engine/Objects/Object.h"
#include "Engine/Log.h"
#include "Engine/Objects/Component.h"
#include "Engine/Luau/LuauScript.h"
#include "Engine/Luau/Lib.h"
#include "Engine/Engine.h"

using namespace andromeda;


// struct RegistryData {
// 	Scene* scene;
// 	uint32_t nextId;
// };
// static std::unordered_map<entt::registry*, RegistryData> s_registryData;

// void added(entt::registry& registry, const entt::entity entity) {
// 	auto& data = s_registryData.at(&registry);
// }

Scene::Scene() {
	// s_registryData.insert({ &m_registry, { this } });
	// m_registry.on_construct<entt::entity>().connect<&added>();
}

Entity Scene::CreateEntity(const std::string& name) {
	return CreateEntity(name, Entity());
}

Entity Scene::CreateEntity(const std::string& name, Entity parent) {
	Entity entity = {this, m_registry.create()};

	auto& name_component = entity.AddComponent<NameComponent>();
	name_component.name = name;

	auto& relationship = entity.AddComponent<EntityRelationships>();
	if (parent) {
		entity.SetParent(parent);
	}

	auto& transform = entity.AddComponent<TransformComponent>();

#if ANDROMEDA_EDITOR
	// In editor the entity is sortable
	auto& sort = entity.AddComponent<EntitySort>(m_sortIdx++);
	m_registry.sort<EntitySort>([](const EntitySort& left, const EntitySort& right) {
		return left.order < right.order;
	});
#endif

	return entity;
}

Entity Scene::CreateEntity() {
	return CreateEntity("Entity", Entity::Null);
}

Entity Scene::CreateEntity(Entity parent) {
	return CreateEntity("Entity", parent);
}

void Scene::SubmitSceneForRendering() {
	auto& engine = andromeda::Engine::GetInstance();
	auto window = engine.GetMainWindow();
	auto renderer = window->GetGraphicsContext();

	renderer->Submit<andromeda::graphics::DrawVkTriangleDemoCommand>();
}

void Scene::DestroyEntity(Entity entity) {
	auto& rel = m_registry.get<EntityRelationships>(entity);

	// Detach from parent
	if (rel.parent != entt::null) {
		auto& prel = m_registry.get<EntityRelationships>(rel.parent);
		auto it = prel.children.find(entity);
		if (it != prel.children.end()) {
			prel.children.erase(it);
		}
	}

	// Clear all children
	for (auto it = rel.children.begin(); it != rel.children.end(); it++) {
		m_registry.destroy(*it);
	}

	m_registry.destroy(entity.GetHandle());

#if ANDROMEDA_EDITOR
	m_registry.sort<EntitySort>([](const EntitySort& left, const EntitySort& right) {
		return left.order < right.order;
	});
#endif
}


void Scene::Initialize() {
	Awake();
	// TODO: Iterate scripts, inject any referent properties
	Start();

	// entt::sigh_helper{m_registry}.
}

void Scene::Sort(entt::entity entity) {}

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

#if ANDROMEDA_EDITOR
void Scene::EditorUpdate(float dt) {
	
}
#endif

void Scene::Update(float dt) {
	if (!m_active) {
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
void Scene::OnComponentAdded<NameComponent>(Entity entity, NameComponent& component) {}

template<>
void Scene::OnComponentAdded<LuauScriptComponent>(Entity entity, LuauScriptComponent& component) {
	component.m_entity = entity;
}

Scene::~Scene() {
	if (m_active)
		Shutdown();

	// s_registryData.erase(&m_registry);
}