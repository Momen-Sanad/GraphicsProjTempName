#pragma once

#include "Player.hpp"

class Crusader : public Player {
public:
    Crusader(
        World& world,
        engine::ecs::EntityId parent,
        std::shared_ptr<MeshRenderer> bodyMesh,
        std::shared_ptr<Material> bodyMaterial,
        std::shared_ptr<MeshRenderer> weaponMesh,
        std::shared_ptr<Material> weaponMaterial);
};
