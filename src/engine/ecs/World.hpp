#pragma once
#ifndef WORLD
#define WORLD 
#include <vector>
#include "Entity.hpp"
#include <algorithm>
#include "../components/Camera.hpp"

class World {
    public:
    Camera* camera;
    std::vector<Entity*> entities;
    
    Entity* add_entity();
    void remove_entity(Entity* entity);
    void update(float deltaTime);
    void render();
};
#endif