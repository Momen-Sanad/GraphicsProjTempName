#include "../engine/ecs/World.hpp"

int main()
{
    World world;
    auto entity = world.createEntity("app-example");
    return world.registry().isAlive(entity) ? 0 : 1;
}
