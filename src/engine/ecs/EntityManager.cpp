#include "EntityManager.hpp"

// ------------------------------------------------------------
// Create entity and store it
// ------------------------------------------------------------
Entity* EntityManager::createEntity() {
    // Create a new unique entity and add it to the entities list
    entities.push_back(std::make_unique<Entity>());
    
    // Get a raw pointer to the entity
    Entity* e = entities.back().get();

    // Add the new entity to the roots list by default
    roots.push_back(e); // Default behavior: the entity is considered a root entity
    return e;           // Return the raw pointer to the newly created entity
}

// ------------------------------------------------------------
// Overloaded creator (factory method)
// ------------------------------------------------------------
Entity* EntityManager::createEntityWithParams(
    Entity* parent,
    const glm::vec3& position,
    const glm::quat& rotation,
    const glm::vec3& scale,
    MeshRenderer* mesh,
    Material* material
) {
    // Call the original zero-argument entity creation function
    Entity* e = createEntity(); 

    // Set the entity's transform (position, rotation, and scale)
    e->setPosition(position);
    e->setRotation(rotation);
    e->setScale(scale);

    // Assign the mesh and material to the entity
    e->setMesh(mesh);
    e->setMaterial(material);

    // Set the parent of the entity, and remove it from the roots list if it has a parent
    if (parent) {
        e->setParent(parent);  // Set the parent entity
        // Remove from roots list if the entity is no longer a root
        roots.erase(std::remove(roots.begin(), roots.end(), e), roots.end());
    }
    
    return e;  // Return the newly created entity
}

// ------------------------------------------------------------
// Create a default cube (helper function, factory)
// ------------------------------------------------------------
Entity* createCube(
    EntityManager& manager,
    Entity* parent,
    const glm::vec3& pos,
    const glm::vec3& scale,
    const glm::quat& rot,
    MeshRenderer* mesh,
    Material* material
) {
    // Create a cube entity using the factory function with parameters
    return manager.createEntityWithParams(parent, pos, rot, scale, mesh, material);
}

// ------------------------------------------------------------
// Remove entity and unparent it
// ------------------------------------------------------------
void EntityManager::destroyEntity(Entity* entity) {
    if (!entity) return;  // If entity is null, nothing to destroy

    // Remove from the root list if the entity is in it
    roots.erase(std::remove(roots.begin(), roots.end(), entity), roots.end());

    // Unset the parent of the entity (if it has one)
    if (entity->getParent())
        entity->setParent(nullptr);

    // Unset the parent references for all of the entity's children
    for (Entity* child : entity->getChildren())
        child->setParent(nullptr);

    // Remove the entity from the list of managed entities
    entities.erase(
        std::remove_if(
            entities.begin(),
            entities.end(),
            [&](const std::unique_ptr<Entity>& e){ return e.get() == entity; }
        ),
        entities.end()
    );
}

// ------------------------------------------------------------
// Clear all entities and reset the root list
// ------------------------------------------------------------
void EntityManager::clear() {
    entities.clear();  // Remove all entities
    roots.clear();     // Remove all root entities
}

// ------------------------------------------------------------
// Draw all root-level entities (children will be drawn recursively)
// ------------------------------------------------------------
void EntityManager::drawAll(const glm::mat4& viewProj) {
    // Iterate through each root entity and draw it
    for (Entity* e : roots)
        e->draw(viewProj);
}

// ------------------------------------------------------------
// Render a single entity recursively (including children)
// ------------------------------------------------------------
void EntityManager::renderEntityRecursive(Entity* e, const glm::mat4& VP) {
    if (!e) return;  // Return if the entity is null

    // Check if the entity has a mesh
    if (MeshRenderer* m = e->getMesh()) {
        // Check if the entity has a material
        if (Material* mat = e->getMaterial()) {
            mat->setup();  // Set up the material (binds the shader and sets uniforms)

            // Compute the world matrix and MVP (Model-View-Projection) matrix
            glm::mat4 M = e->getWorldMatrix();
            glm::mat4 MVP = VP * M;

            // Try to get the MVP uniform location from the material
            GLint loc = mat->getUniform("MVP");
            if (loc != -1)
                glUniformMatrix4fv(loc, 1, GL_FALSE, &MVP[0][0]);  // Set the MVP uniform

            m->draw();  // Draw the mesh
        }
    }

    // Recursively render all child entities
    for (Entity* child : e->getChildren()) {
        EntityManager::renderEntityRecursive(child, VP);  // Recursive call for each child
    }
}
