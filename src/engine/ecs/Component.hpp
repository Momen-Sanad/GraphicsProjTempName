#pragma once
#ifndef COMPONENT
#define COMPONENT
#include <glm/glm.hpp>

class Entity;

class Component {
public:
    virtual void update(Entity& entity, float deltaTime) = 0;
    virtual void render(Entity& entity) = 0;
};
#endif