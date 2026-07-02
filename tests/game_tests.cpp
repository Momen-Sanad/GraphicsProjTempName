#include "Game/Entities/Crusader.hpp"
#include "Game/Entities/Enemy.hpp"
#include "Game/GameEntityFactory.hpp"
#include "Game/GameSystems.hpp"

#include "engine/assets/ModelData.hpp"
#include "engine/assets/SkinnedMaterial.hpp"
#include "engine/assets/TintedMaterial.hpp"
#include "engine/components/Camera.hpp"
#include "engine/components/HealthComponent.hpp"
#include "engine/ecs/EcsComponents.hpp"
#include "engine/ecs/World.hpp"
#include "engine/systems/TransformSystem.hpp"

#include <glm/geometric.hpp>

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

std::shared_ptr<MeshRenderer> inert_renderer()
{
    return std::make_shared<MeshRenderer>();
}

std::shared_ptr<Material> inert_material()
{
    return std::make_shared<TintedMaterial>();
}

void test_game_entity_factory_components()
{
    World world;
    game::GameEntityFactory factory(world);
    auto mesh = inert_renderer();
    auto material = inert_material();

    auto root = factory.createNode("root");
    auto staticEntity = factory.createStaticRenderable("static", mesh, material, root);

    assert_true(world.registry().has<engine::ecs::Transform>(staticEntity), "Static entity should have Transform");
    assert_true(world.registry().has<engine::ecs::Renderable>(staticEntity), "Static entity should have Renderable");
    assert_true(
        world.registry().get<engine::ecs::Hierarchy>(staticEntity)->parent == root,
        "Static entity should be parented to root");

    auto model = std::make_shared<ModelAsset>();
    model->legacyModel = std::make_shared<ModelData>();
    auto skinnedMaterial = std::make_shared<SkinnedMaterial>();
    auto skinned = factory.createSkinnedRenderable("skinned", {}, skinnedMaterial, model, root);
    auto* renderable = world.registry().get<engine::ecs::SkinnedRenderable>(skinned);

    assert_true(renderable != nullptr, "Skinned factory should add SkinnedRenderable");
    assert_true(renderable->model == model, "Skinned factory should keep model handle");
    assert_true(renderable->material == skinnedMaterial, "Skinned factory should keep material handle");
}

void test_player_movement_attack_and_camera()
{
    World world;
    auto root = world.createEntity("root");
    Crusader player(world, root, inert_renderer(), inert_material(), inert_renderer(), inert_material());
    player.setMoveSpeed(4.0f);
    player.setPosition(glm::vec3(0.0f));

    Camera camera;
    player.attachCamera(&camera, glm::vec3(0.0f, 2.0f, 4.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    PlayerInput input;
    input.move.y = -1.0f;
    input.attack = true;
    player.setInput(input);
    player.update(0.25f);

    const glm::vec3 pos = player.getPosition();
    assert_true(pos.z < -0.1f, "Player movement should update ECS transform");
    assert_true(player.isAttacking(), "Attack input should start attack timer");
    assert_true(camera.position.z > pos.z, "Camera follow should update camera position");
}

void test_enemy_ai_moves_and_attacks()
{
    World world;
    auto root = world.createEntity("root");
    auto target = world.createEntity("target", glm::vec3(0.0f));
    Enemy enemy(world, root, inert_renderer(), inert_material());
    enemy.setPosition(glm::vec3(5.0f, 0.0f, 0.0f));
    enemy.setTarget(target);
    enemy.setMoveSpeed(2.0f);
    enemy.setDetectionRange(20.0f);
    enemy.setAttackRange(0.5f);

    const float before = glm::length(enemy.getPosition() - game::worldPosition(world, target));
    enemy.update(0.5f);
    const float after = glm::length(enemy.getPosition() - game::worldPosition(world, target));

    assert_true(after < before, "Enemy should move toward target");

    enemy.setPosition(glm::vec3(0.25f, 0.0f, 0.0f));
    enemy.update(1.0f);
    assert_true(enemy.isAttacking(), "Enemy should attack when inside attack range");
}

void test_xp_level_up()
{
    game::PlayerProgress progress;
    const bool leveled = game::grantExperience(progress, 125);

    assert_true(leveled, "XP grant should report level-up");
    assert_true(progress.level == 2, "Player should advance one level");
    assert_true(progress.xp == 25, "Remaining XP should carry over");
    assert_true(progress.pendingUpgrades == 1, "Level-up should grant upgrade point");
    assert_true(progress.showUpgradeMenu, "Level-up should request upgrade menu");
}

void test_recursive_destroy_invalidates_children()
{
    World world;
    auto root = world.createEntity("root");
    auto child = world.createEntity("child");
    auto grandchild = world.createEntity("grandchild");
    world.setParent(child, root);
    world.setParent(grandchild, child);

    world.destroyEntity(root, DestroyMode::Recursive);

    assert_true(!world.registry().isAlive(root), "Destroyed root handle should be invalid");
    assert_true(!world.registry().isAlive(child), "Recursive destroy should remove child");
    assert_true(!world.registry().isAlive(grandchild), "Recursive destroy should remove grandchild");
}
} // namespace

int main()
{
    run_test("game_entity_factory_components", test_game_entity_factory_components);
    run_test("player_movement_attack_and_camera", test_player_movement_attack_and_camera);
    run_test("enemy_ai_moves_and_attacks", test_enemy_ai_moves_and_attacks);
    run_test("xp_level_up", test_xp_level_up);
    run_test("recursive_destroy_invalidates_children", test_recursive_destroy_invalidates_children);

    std::cout << "Tests passed: " << tests_passed << std::endl;
    std::cout << "Tests failed: " << tests_failed << std::endl;
    return tests_failed == 0 ? 0 : 1;
}
