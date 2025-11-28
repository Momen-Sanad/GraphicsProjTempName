#pragma once
#ifndef ENTITY
#define ENTITY
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

class Component;

class Entity {
public:
    glm::vec3 position;
    glm::quat rotation;
    glm::vec3 scale;

    std::vector<Component*> components;

    Entity() : position(0.0f), rotation(glm::quat(1.0f, 0.0f, 0.0f, 0.0f)), scale(1.0f) {}

    void add_component(Component* component);
    void update(float deltaTime);
    void render();
};
#endif