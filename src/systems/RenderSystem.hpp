/* example usage:

#pragma once
// these includes will be fixed in Cmake
#include "../ecs/SystemManager.hpp"
#include "../components/Transform.hpp"
#include "../components/MeshRenderer.hpp"
#include "../core/MonoBehavviour.hpp"

class RenderSystem : public System {
public:
    void Update(float deltaTime) override {
        for (auto const& entity : entities) {
            auto& transform = gCoordinator.GetComponent<Transform>(entity);
            auto& mesh      = gCoordinator.GetComponent<MeshRenderer>(entity);
            // Call OpenGL rendering logic here
            DrawMesh(mesh, transforma);
        }
    }
};
*/