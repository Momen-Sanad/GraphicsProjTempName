#include "engine/assets/TintedMaterial.hpp"
#include "engine/components/MeshRenderer.hpp"
#include "engine/ecs/EcsComponents.hpp"
#include "engine/ecs/Registry.hpp"
#include "engine/ecs/SystemManager.hpp"
#include "engine/ecs/World.hpp"
#include "engine/systems/TransformSystem.hpp"

#include <glm/gtc/epsilon.hpp>

#include <iostream>
#include <memory>
#include <stdexcept>

namespace {
int tests_passed = 0;
int tests_failed = 0;

void assert_true(bool condition, const char* message)
{
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void assert_close(float actual, float expected, const char* message)
{
    if (glm::abs(actual - expected) > 0.0001f) {
        throw std::runtime_error(message);
    }
}

void run_test(const char* name, void (*test)())
{
    try {
        test();
        std::cout << "[PASS] " << name << std::endl;
        tests_passed++;
    } catch (const std::exception& e) {
        std::cerr << "[FAIL] " << name << " - " << e.what() << std::endl;
        tests_failed++;
    }
}

void test_entity_generation_safety()
{
    engine::ecs::Registry registry;
    engine::ecs::EntityId first = registry.createEntity();
    assert_true(registry.isAlive(first), "New entity should be alive");

    registry.destroyEntity(first);
    assert_true(!registry.isAlive(first), "Destroyed entity handle should be dead");

    engine::ecs::EntityId second = registry.createEntity();
    assert_true(second.index == first.index, "Registry should reuse free entity slots");
    assert_true(second.generation != first.generation, "Reused entity slot should get a new generation");
    assert_true(!registry.isAlive(first), "Old generation should remain invalid after reuse");
    assert_true(registry.isAlive(second), "New generation should be valid");
}

void test_component_add_get_remove()
{
    engine::ecs::Registry registry;
    engine::ecs::EntityId id = registry.createEntity();

    auto& transform = registry.emplace<engine::ecs::Transform>(id);
    transform.position = glm::vec3(2.0f, 3.0f, 4.0f);

    registry.emplace<engine::ecs::Name>(id, engine::ecs::Name{"unit"});

    assert_true(registry.has<engine::ecs::Transform>(id), "Entity should have Transform");
    assert_true(
        registry.matches<engine::ecs::Transform, engine::ecs::Name>(id),
        "Signature should match all present components");
    assert_true(registry.signature(id)->size() == 2, "Signature should track component count");
    assert_true(registry.get<engine::ecs::Name>(id)->value == "unit", "Entity name should round-trip");
    assert_close(registry.get<engine::ecs::Transform>(id)->position.y, 3.0f, "Transform position should round-trip");

    registry.remove<engine::ecs::Name>(id);
    assert_true(!registry.has<engine::ecs::Name>(id), "Removed component should not be queryable");
    assert_true(
        !registry.matches<engine::ecs::Transform, engine::ecs::Name>(id),
        "Signature should stop matching removed components");
}

void test_query_matching()
{
    engine::ecs::Registry registry;
    engine::ecs::EntityId renderable = registry.createEntity();
    engine::ecs::EntityId transformOnly = registry.createEntity();

    registry.emplace<engine::ecs::Transform>(renderable);
    registry.emplace<engine::ecs::Renderable>(renderable);
    registry.emplace<engine::ecs::Transform>(transformOnly);

    int matches = 0;
    registry.each<engine::ecs::Transform, engine::ecs::Renderable>(
        [&matches](engine::ecs::EntityId, engine::ecs::Transform&, engine::ecs::Renderable&) {
            matches++;
        });

    assert_true(matches == 1, "Query should only match entities with all requested components");
}

void test_transform_hierarchy()
{
    World world;
    engine::ecs::EntityId parent = world.createEntity("parent", glm::vec3(10.0f, 0.0f, 0.0f));
    engine::ecs::EntityId child = world.createEntity("child", glm::vec3(5.0f, 0.0f, 0.0f));
    world.setParent(child, parent);

    TransformSystem::updateWorldTransforms(world.registry());

    assert_close(
        world.registry().get<engine::ecs::Transform>(parent)->worldMatrix[3].x,
        10.0f,
        "Parent world transform should include parent translation");
    assert_close(
        world.registry().get<engine::ecs::Transform>(child)->worldMatrix[3].x,
        15.0f,
        "Child world transform should include parent and local translation");
}

void test_world_factory_renderable_handles()
{
    World world;
    auto mesh = std::make_shared<MeshRenderer>();
    auto material = std::make_shared<TintedMaterial>();

    engine::ecs::EntityId entity = world.createRenderable("renderable", mesh, material);
    auto* renderable = world.registry().get<engine::ecs::Renderable>(entity);

    assert_true(renderable != nullptr, "World renderable factory should add Renderable data");
    assert_true(renderable->meshRenderer == mesh, "Renderable should store mesh asset handle");
    assert_true(renderable->material == material, "Renderable should store material asset handle");
}

void test_world_recursive_destroy()
{
    World world;
    engine::ecs::EntityId root = world.createEntity("root");
    engine::ecs::EntityId child = world.createEntity("child");
    engine::ecs::EntityId grandchild = world.createEntity("grandchild");

    world.setParent(child, root);
    world.setParent(grandchild, child);
    world.destroyEntity(root, DestroyMode::Recursive);

    assert_true(!world.registry().isAlive(root), "Root should be destroyed");
    assert_true(!world.registry().isAlive(child), "Recursive destroy should remove child");
    assert_true(!world.registry().isAlive(grandchild), "Recursive destroy should remove grandchild");

    engine::ecs::EntityId reused = world.createEntity("reused");
    assert_true(reused.index == root.index, "Destroyed slot should be available for reuse");
    assert_true(reused.generation != root.generation, "Reused slot should invalidate old handle generation");
}

class CountingSystem final : public engine::ecs::System {
public:
    void update(engine::ecs::Registry&, float deltaTime) override
    {
        updates++;
        totalTime += deltaTime;
    }

    int updates = 0;
    float totalTime = 0.0f;
};

void test_system_manager_updates()
{
    engine::ecs::Registry registry;
    engine::ecs::SystemManager systems;
    CountingSystem& counter = systems.add<CountingSystem>();

    systems.updateAll(registry, 0.25f);
    systems.updateAll(registry, 0.5f);

    assert_true(counter.updates == 2, "SystemManager should update registered systems in order");
    assert_close(counter.totalTime, 0.75f, "SystemManager should pass delta time through");
}
} // namespace

int main()
{
    run_test("entity_generation_safety", test_entity_generation_safety);
    run_test("component_add_get_remove", test_component_add_get_remove);
    run_test("query_matching", test_query_matching);
    run_test("transform_hierarchy", test_transform_hierarchy);
    run_test("world_factory_renderable_handles", test_world_factory_renderable_handles);
    run_test("world_recursive_destroy", test_world_recursive_destroy);
    run_test("system_manager_updates", test_system_manager_updates);

    std::cout << "Tests passed: " << tests_passed << std::endl;
    std::cout << "Tests failed: " << tests_failed << std::endl;
    return tests_failed == 0 ? 0 : 1;
}
