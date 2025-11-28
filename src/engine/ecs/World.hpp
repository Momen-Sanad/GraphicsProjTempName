#ifndef WORLD_HPP
#define WORLD_HPP

#include <vector>
#include "Entity.hpp"
#include "../components/Camera.hpp"

class World {
public:
    World();
    ~World();

    // --- Entity Management ---
    Entity* add_entity();
    void remove_entity(Entity* entity);
    void clear();

    // --- Accessors ---
    const std::vector<Entity*>& get_entities() const { return entities; }
    Camera& get_camera() { return camera; }

private:
    std::vector<Entity*> entities;
    Camera camera;
};

#endif
