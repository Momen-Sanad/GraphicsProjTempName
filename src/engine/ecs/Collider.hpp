#pragma once

#include <glm/glm.hpp>

class Entity;

struct AABB {
    glm::vec3 min;
    glm::vec3 max;
};

class Collider {
public:
    Collider() = default;
    explicit Collider(const glm::vec3& extents,
                      const glm::vec3& offset = glm::vec3(0.0f));

    void setParent(Entity* entity) { parent = entity; }
    Entity* getParent() const { return parent; }

    void setHalfExtents(const glm::vec3& extents) { halfExtents = extents; }
    const glm::vec3& getHalfExtents() const { return halfExtents; }

    void setLocalOffset(const glm::vec3& offset) { localOffset = offset; }
    const glm::vec3& getLocalOffset() const { return localOffset; }

    void setEnabled(bool value) { enabled = value; }
    bool isEnabled() const { return enabled; }

    AABB getWorldAABB() const;
    bool intersects(const Collider& other) const;

private:
    Entity* parent = nullptr;
    glm::vec3 halfExtents{0.5f, 0.5f, 0.5f};
    glm::vec3 localOffset{0.0f, 0.0f, 0.0f};
    bool enabled{true};
};
