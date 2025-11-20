#pragma once

class Entity;

class Component {
public:
    virtual void initialize(Entity* entity) = 0;
    virtual void update(float dt) = 0;
};
