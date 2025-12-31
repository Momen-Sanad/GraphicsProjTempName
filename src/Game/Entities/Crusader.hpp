#pragma once
#include "Player.hpp"

class Crusader : public Player {
public:
    Crusader(World& world,
             Entity* parent,
             GpuMesh* bodyMesh,
             Material* bodyMaterial,
             GpuMesh* weaponMesh,
             Material* weaponMaterial);
};
