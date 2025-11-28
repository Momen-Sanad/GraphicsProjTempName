#include "World.hpp"

// ------------------------------------------------------
// Constructor / Destructor
// ------------------------------------------------------

World::World() {
    // You can place default world setup here (optional)
}

World::~World() {
    clear();
}

// ------------------------------------------------------
// Add an Entity
// ------------------------------------------------------

Entity* World::add_entity() {
    Entity* e = new Entity();
    entities.push_back(e);
    return e;
}

// ------------------------------------------------------
// Remove Entity safely
// ------------------------------------------------------

void World::remove_entity(Entity* entity) {
    if (!entity) return;

    // Fix children relationships BEFORE erasing
    for (Entity* e : entities) {
        if (e->getParent() == entity)
            e->setParent(entity->getParent());
    }

    // Find and erase
    for (auto it = entities.begin(); it != entities.end(); ++it) {
        if (*it == entity) {
            delete entity;
            entities.erase(it);
            return;
        }
    }
}

// ------------------------------------------------------
// Clear entire world
// ------------------------------------------------------

void World::clear() {
    for (Entity* e : entities)
        delete e;

    entities.clear();
}
