#include "Engine/Objects/Object.h"
#include "Engine/Objects/Component.h"
using namespace andromeda;



void Entity::SetParent(const Entity& parent) {
    auto& childRel = GetComponent<EntityRelationship>();
    childRel.parent = parent.m_entity;
    
    auto& parentRel = parent.GetComponent<EntityRelationship>();
    parentRel.AddChild(m_scene->m_registry, m_entity);
}

Entity Entity::GetParent() const {
    auto& childRel = GetComponent<EntityRelationship>();
    if (childRel.parent != entt::null) {
        return Entity(m_scene, childRel.parent);
    }

    return Entity();
}

std::vector<Entity> Entity::GetChildren() const {
    auto& rel = m_scene->m_registry.get<EntityRelationship>(m_entity);

    std::vector<Entity> children;
    children.resize(rel.children);

    auto curr = rel.first;
    for (std::size_t i{}; i < rel.children; ++i) {
        children[i] = Entity(m_scene, curr);
        curr = m_scene->m_registry.get<EntityRelationship>(curr).next;
    }

    return children;
}