#include "Engine/Objects/Object.h"
#include "Engine/Scene/Scene.h"

template<typename T, typename... Args>
T& andromeda::Entity::AddComponent(Args&&... args) {
    T& component = m_scene->m_registry.emplace<T>(m_entity, std::forward<Args>(args)...);
    return component;
}

template<typename T>
T& andromeda::Entity::GetComponent() {
    return m_scene->m_registry.get<T>(m_entity);
}