#pragma once

class World;
class Entity;
class SpawnPointComponent;

class SpawnSystem {
public:
    void update(World& world, float deltaTime);

private:
    void update_entity_recursive(Entity& entity, World& world, float deltaTime);
    void update_spawn_point(Entity& entity, World& world, SpawnPointComponent& spawnPoint, float deltaTime);
};
