#pragma once
#ifndef COMPONENT_HPP
#define COMPONENT_HPP

class Entity;

class Component {
public:
    explicit Component(Entity* owner)
        : owner(owner) {}

    virtual ~Component() = default;

    Entity* getOwner() const { return owner; }

    // Called every frame by a System
    virtual void update(float deltaTime) {}
    virtual void render(Entity& entity) = 0;

    // lifecycle hooks
    virtual void onAdded() {}
    virtual void onRemoved() {}

protected:
    Entity* owner;
};

#endif