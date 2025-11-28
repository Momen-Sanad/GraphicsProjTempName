#include "EntityManager.hpp"
#include <algorithm>

// ------------------------------------------------------------
// Create entity and store it
// ------------------------------------------------------------
Entity* EntityManager::createEntity() {
    entities.push_back(std::make_unique<Entity>());
    Entity* e = entities.back().get();
    roots.push_back(e); // default → root entity
    return e;
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

    // Remove all children’s parent references
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

// ------------------------------------------------------------
// Draw all root-level entities (children auto-draw recursively)
// ------------------------------------------------------------
void EntityManager::drawAll(const glm::mat4& viewProj) {
    for (Entity* e : roots)
        e->draw(viewProj);
}
