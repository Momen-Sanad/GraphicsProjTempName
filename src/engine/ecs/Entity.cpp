#include "Entity.hpp"
#include "TransformComponent.hpp"
#include "Component.hpp"

// Entity constructor
Entity::Entity() : position(0.0f), rotation(glm::quat(1.0f, 0.0f, 0.0f, 0.0f)), scale(1.0f) {}

// Add a component to the entity
void Entity::add_component(Component* component) {
    components.push_back(component);
}

// Update all components of the entity
void Entity::update(float deltaTime) {
    for (auto component : components) {
        component->update(*this, deltaTime);  // Call the derived component's update method
    }
}

// Render all components of the entity
void Entity::render() {
    for (auto component : components) {
        component->render(*this);  // Call the derived component's render method
    }
}
