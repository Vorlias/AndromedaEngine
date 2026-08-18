#include "Engine/Objects/Object.h"
#include "Engine/Objects/Component.h"
using namespace andromeda;

void Entity::SetParent(const Entity& parent) {
    auto& childRel = GetComponent<EntityRelationships>();
    childRel.parent = parent.m_entity;
    
    auto& parentRel = parent.GetComponent<EntityRelationships>();
    parentRel.AddChild(m_scene->m_registry, m_entity);
}

Entity Entity::GetParent() const {
    auto& childRel = GetComponent<EntityRelationships>();
    if (childRel.parent != entt::null) {
        return Entity(m_scene, childRel.parent);
    }

    return Entity();
}

const std::vector<Entity> Entity::GetChildren() const {
    auto& rel = m_scene->m_registry.get<EntityRelationships>(m_entity);

    std::vector<Entity> children;
    children.resize(rel.childCount);

    auto curr = rel.firstChild;
    for (std::size_t i{}; i < rel.childCount; ++i) {
        children[i] = Entity(m_scene, curr);
        curr = m_scene->m_registry.get<EntityRelationships>(curr).nextSibling;
    }

    return children;
}
