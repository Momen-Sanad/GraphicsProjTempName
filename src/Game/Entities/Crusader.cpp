#include "Crusader.hpp"

#include <utility>

Crusader::Crusader(
    World& world,
    engine::ecs::EntityId parent,
    std::shared_ptr<MeshRenderer> bodyMesh,
    std::shared_ptr<Material> bodyMaterial,
    std::shared_ptr<MeshRenderer> weaponMesh,
    std::shared_ptr<Material> weaponMaterial)
    : Player(
          world,
          parent,
          std::move(bodyMesh),
          std::move(bodyMaterial),
          std::move(weaponMesh),
          std::move(weaponMaterial))
{
    setMoveSpeed(3.5f);
    setBlockSpeedMultiplier(0.3f);
    setAttackTimings(0.3f, 0.5f);
    setDodgeTimings(0.2f, 0.9f);
}
