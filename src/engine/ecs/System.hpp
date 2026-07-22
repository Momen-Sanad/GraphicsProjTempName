#pragma once

namespace engine::ecs {

class Registry;

class System {
public:
    virtual ~System() = default;
    virtual void update(Registry& registry, float deltaTime) = 0;
};

} // namespace engine::ecs
