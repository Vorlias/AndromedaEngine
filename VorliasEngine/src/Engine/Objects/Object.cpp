#include "Engine/Objects/Object.h"
#include "Engine/Objects/Component.h"
using namespace andromeda;

void Entity::SetParent(const Entity& parent) {
	auto& childRel = GetComponent<EntityRelationships>();
	childRel.parent = parent.m_entity;

	auto& parentRel = parent.GetComponent<EntityRelationships>();
	parentRel.children.push_back(m_entity);
}

Entity Entity::GetParent() const {
	auto& childRel = GetComponent<EntityRelationships>();
	if (childRel.parent != entt::null) {
		return Entity(m_scene, childRel.parent);
	}

	return Entity();
}

const std::vector<Entity>& Entity::GetChildren() const {
	auto& r = m_scene->m_registry.get<EntityRelationships>(m_entity);

	std::vector<Entity> children;
	children.resize(r.children.size());

	int i = 0;
	for (auto it = r.children.begin(); it != r.children.end(); it++) {
		children[i] = Entity(m_scene, *it);
		i++;
	}

	return std::move(children);
}

const andromeda::List<Entity> Entity::GetDescendants() const {
	auto& r = m_scene->m_registry.get<EntityRelationships>(m_entity);

	andromeda::List<Entity> descendants;

    for (auto it = r.children.begin(); it != r.children.end(); ++it) {
        Entity entity(m_scene, *it);
        descendants.push_back(entity);
        descendants.push_back(entity.GetDescendants());
    }

	return std::move(descendants);
}