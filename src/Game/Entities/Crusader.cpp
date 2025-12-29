#include "Crusader.hpp"

Crusader::Crusader(World& world,
                   Entity* parent,
                   MeshRenderer* bodyMesh,
                   Material* bodyMaterial,
                   MeshRenderer* weaponMesh,
                   Material* weaponMaterial)
    : Player(world, parent, bodyMesh, bodyMaterial, weaponMesh, weaponMaterial) {

    setMoveSpeed(3.5f);
    setBlockSpeedMultiplier(0.3f);
    setAttackTimings(0.3f, 0.5f);
    setDodgeTimings(0.2f, 0.9f);
}