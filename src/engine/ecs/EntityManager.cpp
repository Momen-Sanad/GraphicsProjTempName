#include "EntityManager.hpp"
#include <algorithm>

// ------------------------------------------------------------
// Create entity and store it
// ------------------------------------------------------------
Entity* EntityManager::createEntity() {
    entities.push_back(std::make_unique<Entity>());
    Entity* e = entities.back().get();
    roots.push_back(e); // default -> root entity
    return e;
}

//overloaded creator (factory)
Entity* EntityManager::createEntityWithParams(
    Entity* parent,
    const glm::vec3& position,
    const glm::quat& rotation,
    const glm::vec3& scale,
    MeshRenderer* mesh,
    Material* material
) {
    // Call the original zero-argument function
    Entity* e = createEntity(); 

    // Set transforms
    e->setPosition(position);
    e->setRotation(rotation);
    e->setScale(scale);

    // Set mesh/material
    e->setMesh(mesh);
    e->setMaterial(material);

    // Set parent (removes from roots if parent != nullptr)
    if (parent){
        e->setParent(parent);
        roots.erase(std::remove(roots.begin(), roots.end(), e), roots.end());
    }
    
    return e;
}


Entity* createCube(
    EntityManager& manager,
    Entity* parent,
    const glm::vec3& pos,
    const glm::vec3& scale,
    const glm::quat& rot,
    MeshRenderer* mesh,
    Material* material
) {
    return manager.createEntityWithParams(parent, pos, rot, scale, mesh, material);
}

// ------------------------------------------------------------
// Remove entity and unparent it
// ------------------------------------------------------------
void EntityManager::destroyEntity(Entity* entity) {
    if (!entity) return;

    // Remove from root list if present
    roots.erase(std::remove(roots.begin(), roots.end(), entity), roots.end());

    // Remove from parent if needed
    if (entity->getParent())
        entity->setParent(nullptr);

    // Remove all childrens' parent references
    for (Entity* child : entity->getChildren())
        child->setParent(nullptr);

    entities.erase(
        std::remove_if(
            entities.begin(),
            entities.end(),
            [&](const std::unique_ptr<Entity>& e){ return e.get() == entity; }
        ),
        entities.end()
    );
}

void EntityManager::clear() {
    entities.clear();
    roots.clear();
}

// ------------------------------------------------------------
// Draw all root-level entities (children auto-draw recursively)
// ------------------------------------------------------------
void EntityManager::drawAll(const glm::mat4& viewProj) {
    for (Entity* e : roots)
        e->draw(viewProj);
}

void EntityManager::renderEntityRecursive(Entity* e, const glm::mat4& VP) {
    if (!e) return;

    if (MeshRenderer* m   = e->getMesh()) {
        if (Material* mat = e->getMaterial()) {
            mat->setup();

            glm::mat4 M = e->getWorldMatrix();
            glm::mat4 MVP = VP * M;

            GLint loc = mat->getUniform("MVP");
            if (loc != -1) glUniformMatrix4fv(loc, 1, GL_FALSE, &MVP[0][0]);

            m->draw();
        }
    }

    for (Entity* child : e->getChildren()) {
        EntityManager::renderEntityRecursive(child, VP);
    }
}