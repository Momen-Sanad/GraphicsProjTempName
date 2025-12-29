#pragma once
#include "Player.hpp"

class Crusader : public Player {
public:
    Crusader(World& world,
             Entity* parent,
             MeshRenderer* bodyMesh,
             Material* bodyMaterial,
             MeshRenderer* weaponMesh,
             Material* weaponMaterial);
};