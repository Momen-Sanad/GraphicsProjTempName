/* example usage:

#pragma once
// includes will be fixed in CMake
#include "../ecs/Coordinator.hpp"
#include "../systems/RenderSystem.hpp"
#include "MonoBehaviour.hpp"

class Engine {
public:
    void Init() {
        gCoordinator.Init();

        gCoordinator.RegisterComponent<Transform>();
        gCoordinator.RegisterComponent<MeshRenderer>();

        renderSystem = gCoordinator.RegisterSystem<RenderSystem>();
        Signature renderSig;
        renderSig.set(gCoordinator.GetComponentType<Transform>());
        renderSig.set(gCoordinator.GetComponentType<MeshRenderer>());
        gCoordinator.SetSystemSignature<RenderSystem>(renderSig);
    }

    void Update(float dt) {
        renderSystem->Update(dt);
    }

private:
    std::shared_ptr<RenderSystem> renderSystem;
};

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

now main.cpp can be like this:

Coordinator gCoordinator; // global instance

int main() {
    Application app;
    app.Run();
    return 0;
}

*/