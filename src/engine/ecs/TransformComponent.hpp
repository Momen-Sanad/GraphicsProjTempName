#pragma once
#ifndef TRANSFORM_COMPONENT
#define TRANSFORM_COMPONENT

#include "Component.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

class Entity;

class TransformComponent {
public:
    glm::vec3 position;
    glm::quat rotation;
    glm::vec3 scale;

    TransformComponent()
        : position(0.0f), rotation(glm::quat(1.0f, 0.0f, 0.0f, 0.0f)), scale(1.0f) {}

    void update(Entity& entity, float deltaTime);
    void render(Entity& entity);

    // Transform operations
    void translate(const glm::vec3& translation);
    void rotate(const glm::vec3& axis, float angle);
    void set_scale(const glm::vec3& scaleFactor);

    // Getter methods for the transform
    const glm::vec3& get_position() const { return position; }
    const glm::quat& get_rotation() const { return rotation; }
    const glm::vec3& get_scale() const { return scale; }
};

#endif
