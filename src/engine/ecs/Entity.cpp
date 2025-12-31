#include "Entity.hpp"
#include "World.hpp"

Entity::Entity(EntityId id, World* world)
    : m_id(id), m_world(world) {}

bool Entity::isValid() const {
    return m_world && m_world->isAlive(m_id);
}

Transform& Entity::transform() {
    return getComponent<Transform>();
}

const Transform& Entity::transform() const {
    return getComponent<Transform>();
}

void Entity::setParent(Entity* newParent) {
    auto& tr = transform();
    tr.parent = newParent ? newParent->id() : kInvalidEntity;
}

Entity* Entity::getParent() const {
    const auto& tr = transform();
    if (tr.parent == kInvalidEntity) {
        return nullptr;
    }
    return m_world->getEntity(tr.parent);
}

std::vector<Entity*> Entity::getChildren() const {
    return m_world->getChildren(m_id);
}

void Entity::setPosition(const glm::vec3& p) {
    transform().position = p;
}

void Entity::setRotation(const glm::quat& q) {
    transform().rotation = q;
}

void Entity::setScale(const glm::vec3& s) {
    transform().scale = s;
}

glm::vec3 Entity::getPosition() const {
    return transform().position;
}

glm::quat Entity::getRotation() const {
    return transform().rotation;
}

glm::vec3 Entity::getScale() const {
    return transform().scale;
}

void Entity::rotateBy(const glm::quat& dq) {
    transform().rotation = dq * transform().rotation;
}

glm::mat4 Entity::getLocalMatrix() const {
    const auto& tr = transform();
    glm::mat4 T = glm::translate(glm::mat4(1.0f), tr.position);
    glm::mat4 R = glm::mat4_cast(tr.rotation);
    glm::mat4 S = glm::scale(glm::mat4(1.0f), tr.scale);
    return T * R * S;
}

glm::mat4 Entity::getWorldMatrix() const {
    Entity* parent = getParent();
    if (!parent) {
        return getLocalMatrix();
    }
    return parent->getWorldMatrix() * getLocalMatrix();
}
