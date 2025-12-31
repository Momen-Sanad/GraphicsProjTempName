#include "World.hpp"

// ------------------------------------------------------------
// Constructor for World class (default constructor)
// ------------------------------------------------------------
World::World() = default;  // Uses default constructor

// ------------------------------------------------------------
// Create an entity with specified parameters
// ------------------------------------------------------------
Entity* World::createEntityWithParams(Entity* parent,
                                      const glm::vec3& position,
                                      const glm::quat& rotation,
                                      const glm::vec3& scale,
                                      MeshRenderer* mesh,
                                      Material* material)
{
    // Use the EntityManager to create an entity with the specified parameters
    Entity* e = manager.createEntityWithParams(
        parent,  // Set the parent entity (can be nullptr)
        position,  // Position of the entity in world space
        glm::quat(1,0,0,0),  // Always pass identity quaternion (no rotation) as a default
        scale,  // Scale of the entity
        mesh,  // MeshRenderer to be used with this entity
        material  // Material to be applied to the entity
    );

    // If the provided rotation is significant (non-zero), set it to the entity
    if (glm::length(rotation) > 0.0001f) {
        e->setRotation(rotation);  // Apply the given rotation to the entity
    }

    return e;  // Return the newly created entity
}

// ------------------------------------------------------------
// Add a new entity to the world (default root entity)
// ------------------------------------------------------------
Entity* World::add_entity() {
    return manager.createEntity();  // Creates a default root entity
}

// ------------------------------------------------------------
// Remove an entity from the world
// ------------------------------------------------------------
void World::removeEntity(Entity* entity) {
    manager.destroyEntity(entity);  // Delegate the removal of the entity to the EntityManager
}

// ------------------------------------------------------------
// Clear all entities in the world (destroy all entities)
// ------------------------------------------------------------
void World::clear() {
    manager.clear();  // Calls the EntityManager to clear all entities
}
