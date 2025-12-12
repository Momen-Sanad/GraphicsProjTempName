#include "TransformComponent.hpp"
#include "Entity.hpp"
#include <iostream>

// ------------------------------------------------------------
// Update function to apply transformations over time
// ------------------------------------------------------------
void TransformComponent::update(Entity& entity, float deltaTime) {
    // This function is meant for applying automatic transformations to an entity over time.
    // For example, we could make the entity rotate around the Y-axis every frame.

    glm::quat rot = entity.getRotation();  // Get the current rotation of the entity
    // Apply a rotation around the Y-axis by 30 degrees per second (scaled by deltaTime for frame-rate independence)
    rot = glm::rotate(rot, glm::radians(30.f) * deltaTime, glm::vec3(0.f, 1.f, 0.f));
    entity.setRotation(rot);  // Set the updated rotation back to the entity

    // Additional transformation logic can be added here:
    // - Apply movement, scaling, or other transformations.
    // - For example, the position of the entity could be updated over time based on velocity or other factors.
}

// ------------------------------------------------------------
// Render function to print the entity's current transform
// ------------------------------------------------------------
void TransformComponent::render(Entity& entity) {
    // This function would normally be used to apply transformations to meshes before rendering.
    // Here, we just print the entity's transform to the console.

    std::cout << "Entity Transform: "
              << "Position (" << position.x << ", " << position.y << ", " << position.z << "), "
              << "Rotation (" << rotation.x << ", " << rotation.y << ", " << rotation.z << ", " << rotation.w << "), "
              << "Scale (" << scale.x << ", " << scale.y << ", " << scale.z << ")" << std::endl;
}

// ------------------------------------------------------------
// Translate the entity by a given translation vector
// ------------------------------------------------------------
void TransformComponent::translate(const glm::vec3& translation) {
    // This function modifies the position of the entity by adding the translation vector.
    position += translation;  // Move the entity by the translation vector
}

// ------------------------------------------------------------
// Apply a rotation to the entity around a given axis
// ------------------------------------------------------------
void TransformComponent::rotate(const glm::vec3& axis, float angle) {
    // This function rotates the entity around a given axis by the specified angle (in radians).
    rotation = glm::rotate(rotation, angle, axis);  // Apply the rotation to the existing rotation quaternion
}

// ------------------------------------------------------------
// Set the entity's scale by multiplying the current scale by a scale factor
// ------------------------------------------------------------
void TransformComponent::set_scale(const glm::vec3& scaleFactor) {
    scale *= scaleFactor;  // Scale the entity by the provided factor
}
