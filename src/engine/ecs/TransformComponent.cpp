#include "TransformComponent.hpp"
#include "Entity.hpp"
#include <iostream>

void TransformComponent::update(Entity& entity, float deltaTime) {
    // You could apply automatic movement or other transformations over time here.
    // This is an example where we could move the entity along the x-axis:
    position.x += 1.0f * deltaTime;  // Example movement along x-axis

    // If you had rotation or scaling logic here, you could apply it as well.
    // For example, you could rotate the entity around the y-axis over time:
    // rotate(glm::vec3(0.0f, 1.0f, 0.0f), glm::radians(30.0f) * deltaTime); // Rotate 30 degrees per second

    // We don't apply any rendering logic in the update function, just transformations.
}

void TransformComponent::render(Entity& entity) {
    // In this method, we will just print out the entity's transform for now
    // In a real scenario, you would apply the transform to a mesh before rendering

    std::cout << "Entity Transform: "
              << "Position (" << position.x << ", " << position.y << ", " << position.z << "), "
              << "Rotation (" << rotation.x << ", " << rotation.y << ", " << rotation.z << ", " << rotation.w << "), "
              << "Scale (" << scale.x << ", " << scale.y << ", " << scale.z << ")" << std::endl;

    // In real rendering, you'd apply position, rotation, scale to a model and then render it
    // For example:
    // model->set_position(position);
    // model->set_rotation(rotation);
    // model->set_scale(scale);
    // model->draw();
}

void TransformComponent::translate(const glm::vec3& translation) {
    position += translation;
}

void TransformComponent::rotate(const glm::vec3& axis, float angle) {
    // Apply rotation to the existing rotation quaternion
    rotation = glm::rotate(rotation, angle, axis);
}

void TransformComponent::scale(const glm::vec3& scaleFactor) {
    scale *= scaleFactor;  // Scale each axis by the provided factor
}
