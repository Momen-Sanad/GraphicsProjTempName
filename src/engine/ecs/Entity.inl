#pragma once

template <typename T, typename... Args>
T& Entity::addComponent(Args&&... args) {
    return m_world->addComponent<T>(m_id, std::forward<Args>(args)...);
}

template <typename T>
bool Entity::hasComponent() const {
    return m_world->hasComponent<T>(m_id);
}

template <typename T>
T& Entity::getComponent() {
    return m_world->getComponent<T>(m_id);
}

template <typename T>
const T& Entity::getComponent() const {
    return m_world->getComponent<T>(m_id);
}

template <typename T>
void Entity::removeComponent() {
    m_world->removeComponent<T>(m_id);
}
