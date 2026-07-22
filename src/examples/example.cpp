#include "../engine/ecs/World.hpp"

int main()
{
    World world;
    auto root = world.createEntity("example-root");
    auto child = world.createEntity("example-child");
    world.setParent(child, root);
    return world.registry().isAlive(child) ? 0 : 1;
}
