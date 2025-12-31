#pragma once

#include "Types.hpp"
#include "../components/Transform.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <utility>
#include <vector>

class World;

class Entity {
public:
    Entity(EntityId id, World* world);

    EntityId id() const { return m_id; }
    bool isValid() const;

    template <typename T, typename... Args>
    T& addComponent(Args&&... args);

    template <typename T>
    bool hasComponent() const;

    template <typename T>
    T& getComponent();

    template <typename T>
    const T& getComponent() const;

    template <typename T>
    void removeComponent();

    Transform& transform();
    const Transform& transform() const;

    void setParent(Entity* newParent);
    Entity* getParent() const;
    std::vector<Entity*> getChildren() const;

    void setPosition(const glm::vec3& p);
    void setRotation(const glm::quat& q);
    void setScale(const glm::vec3& s);

    glm::vec3 getPosition() const;
    glm::quat getRotation() const;
    glm::vec3 getScale() const;

    void rotateBy(const glm::quat& dq);

    glm::mat4 getLocalMatrix() const;
    glm::mat4 getWorldMatrix() const;

private:
    EntityId m_id;
    World* m_world = nullptr;
};

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
