#pragma once

#include "GameComponents.hpp"

#include "../engine/components/CombatComponent.hpp"

class World;

namespace game {

glm::vec3 localPosition(World& world, engine::ecs::EntityId entity);
glm::vec3 worldPosition(World& world, engine::ecs::EntityId entity);
glm::vec3 forward(World& world, engine::ecs::EntityId entity);

void setLocalPosition(World& world, engine::ecs::EntityId entity, const glm::vec3& position);
void updatePlayerController(World& world, engine::ecs::EntityId player, float deltaTime);
void updateEnemyAI(World& world, engine::ecs::EntityId enemy, float deltaTime);
DefenseState defenseState(World& world, engine::ecs::EntityId player);
bool grantExperience(PlayerProgress& progress, int xpValue);

} // namespace game
