#include "engine/assets/AssetManager.hpp"
#include "engine/assets/TintedMaterial.hpp"
#include "engine/components/MeshRenderer.hpp"
#include "engine/ecs/EcsComponents.hpp"
#include "engine/ecs/Registry.hpp"
#include "engine/ecs/SystemManager.hpp"
#include "engine/ecs/World.hpp"
#include "engine/systems/BoneAttachmentSystem.hpp"
#include "engine/systems/PhysicsCollisionSystem.hpp"
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
    assert_true(world.setParent(child, parent), "Valid parent relationship should succeed");

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

void test_hierarchy_rejects_cycles()
{
    World world;
    engine::ecs::EntityId root = world.createEntity("root");
    engine::ecs::EntityId child = world.createEntity("child");
    engine::ecs::EntityId grandchild = world.createEntity("grandchild");

    assert_true(world.setParent(child, root), "Root should accept child");
    assert_true(world.setParent(grandchild, child), "Child should accept grandchild");
    assert_true(!world.setParent(root, grandchild), "Parenting an ancestor under a descendant should fail");
    assert_true(!world.setParent(root, root), "Self-parenting should fail");

    const auto* rootHierarchy = world.registry().get<engine::ecs::Hierarchy>(root);
    const auto* childHierarchy = world.registry().get<engine::ecs::Hierarchy>(child);
    assert_true(rootHierarchy && !rootHierarchy->parent.valid(), "Failed parent operation should not mutate root parent");
    assert_true(childHierarchy && childHierarchy->parent == root, "Existing child parent should remain intact");

    world.destroyEntity(root, DestroyMode::Recursive);
    assert_true(!world.registry().isAlive(root), "Recursive destroy should remove root after rejected cycle");
    assert_true(!world.registry().isAlive(child), "Recursive destroy should remove child after rejected cycle");
    assert_true(!world.registry().isAlive(grandchild), "Recursive destroy should remove grandchild after rejected cycle");
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

    assert_true(world.setParent(child, root), "Child parent should be assigned");
    assert_true(world.setParent(grandchild, child), "Grandchild parent should be assigned");
    world.destroyEntity(root, DestroyMode::Recursive);

    assert_true(!world.registry().isAlive(root), "Root should be destroyed");
    assert_true(!world.registry().isAlive(child), "Recursive destroy should remove child");
    assert_true(!world.registry().isAlive(grandchild), "Recursive destroy should remove grandchild");

    engine::ecs::EntityId reused = world.createEntity("reused");
    assert_true(reused.index == root.index, "Destroyed slot should be available for reuse");
    assert_true(reused.generation != root.generation, "Reused slot should invalidate old handle generation");
}

void add_collider(World& world, engine::ecs::EntityId entity, const glm::vec3& halfExtents)
{
    auto& collider = world.registry().emplace<engine::ecs::ColliderData>(entity);
    collider.halfExtents = halfExtents;
}

void test_collision_refreshes_world_transform()
{
    World world;
    engine::ecs::EntityId mover = world.createEntity("mover", glm::vec3(0.0f));
    engine::ecs::EntityId obstacle = world.createEntity("obstacle", glm::vec3(0.75f, 0.0f, 0.0f));
    add_collider(world, mover, glm::vec3(0.5f));
    add_collider(world, obstacle, glm::vec3(0.5f));

    const bool resolved = PhysicsCollisionSystem::resolveStaticCollision(world.registry(), mover, obstacle);
    const auto* transform = world.registry().get<engine::ecs::Transform>(mover);

    assert_true(resolved, "Overlapping colliders should resolve");
    assert_close(transform->position.x, -0.25f, "Mover local position should be corrected");
    assert_close(transform->worldMatrix[3].x, -0.25f, "World matrix should refresh after collision correction");
}

void test_collision_correction_respects_parent_space()
{
    World world;
    engine::ecs::EntityId parent = world.createEntity(
        "parent",
        glm::vec3(10.0f, 0.0f, 0.0f),
        glm::quat(1.0f, 0.0f, 0.0f, 0.0f),
        glm::vec3(2.0f, 1.0f, 1.0f));
    engine::ecs::EntityId mover = world.createEntity("mover", glm::vec3(0.0f));
    engine::ecs::EntityId obstacle = world.createEntity("obstacle", glm::vec3(10.75f, 0.0f, 0.0f));
    assert_true(world.setParent(mover, parent), "Mover should be parented for local-space collision test");
    add_collider(world, mover, glm::vec3(0.5f));
    add_collider(world, obstacle, glm::vec3(0.5f));

    const bool resolved = PhysicsCollisionSystem::resolveStaticCollision(world.registry(), mover, obstacle);
    const auto* transform = world.registry().get<engine::ecs::Transform>(mover);

    assert_true(resolved, "Parented overlapping colliders should resolve");
    assert_close(transform->position.x, -0.375f, "World correction should be converted into parent local space");
    assert_close(transform->worldMatrix[3].x, 9.25f, "Parented mover world matrix should refresh after correction");
}

void test_sequential_collision_resolution_uses_fresh_transforms()
{
    World world;
    engine::ecs::EntityId mover = world.createEntity("mover", glm::vec3(0.0f));
    engine::ecs::EntityId obstacleX = world.createEntity("obstacle-x", glm::vec3(0.75f, 0.0f, 0.0f));
    engine::ecs::EntityId obstacleZ = world.createEntity("obstacle-z", glm::vec3(-0.25f, 0.0f, 0.75f));
    add_collider(world, mover, glm::vec3(0.5f));
    add_collider(world, obstacleX, glm::vec3(0.5f));
    add_collider(world, obstacleZ, glm::vec3(0.5f));

    assert_true(
        PhysicsCollisionSystem::resolveStaticCollision(world.registry(), mover, obstacleX),
        "First overlap should resolve");
    assert_true(
        PhysicsCollisionSystem::resolveStaticCollision(world.registry(), mover, obstacleZ),
        "Second overlap should resolve using refreshed world transform");

    const auto* transform = world.registry().get<engine::ecs::Transform>(mover);
    assert_close(transform->position.x, -0.25f, "First correction should remain applied");
    assert_close(transform->position.z, -0.25f, "Second correction should use updated mover position");
    assert_close(transform->worldMatrix[3].z, -0.25f, "World matrix should refresh after sequential corrections");
}

std::shared_ptr<ModelAsset> make_attachment_model()
{
    auto model = std::make_shared<ModelAsset>();
    auto skeleton = std::make_shared<Skeleton>();
    skeleton->add_bone("root", -1, glm::mat4(1.0f));
    skeleton->add_bone("hand", 0, glm::mat4(1.0f));

    auto clip = std::make_shared<AnimationClip>("pose", 1.0f, 1.0f);
    BoneAnimation root;
    root.bone_id = 0;
    root.position_keys.push_back({0.0f, glm::vec3(0.0f)});
    root.rotation_keys.push_back({0.0f, glm::quat(1.0f, 0.0f, 0.0f, 0.0f)});
    root.scale_keys.push_back({0.0f, glm::vec3(1.0f)});
    clip->add_bone_animation(root);

    BoneAnimation hand;
    hand.bone_id = 1;
    hand.position_keys.push_back({0.0f, glm::vec3(0.0f, 2.0f, 0.0f)});
    hand.rotation_keys.push_back({0.0f, glm::quat(1.0f, 0.0f, 0.0f, 0.0f)});
    hand.scale_keys.push_back({0.0f, glm::vec3(1.0f)});
    clip->add_bone_animation(hand);

    model->skins.push_back(SkinAsset{"test", skeleton, {0, 1}});
    model->animations.push_back(clip);
    return model;
}

void test_bone_attachment_follows_parented_source_bone()
{
    World world;
    auto model = make_attachment_model();

    engine::ecs::EntityId source = world.createEntity("source", glm::vec3(5.0f, 0.0f, 0.0f));
    auto& animation = world.registry().emplace<engine::ecs::AnimatorData>(source);
    animation.model = model;
    animation.skinIndex = 0;
    animation.animator.set_skeleton(model->skins.front().skeleton.get());
    animation.animator.play(model->animations.front().get(), true);

    engine::ecs::EntityId parent = world.createEntity("parent", glm::vec3(1.0f, 0.0f, 0.0f));
    engine::ecs::EntityId target = world.createEntity("target");
    assert_true(world.setParent(target, parent), "Attachment target should be parented");

    auto& attachment = world.registry().emplace<engine::ecs::BoneAttachment>(target);
    attachment.sourceEntity = source;
    attachment.boneName = "hand";
    attachment.localOffset = glm::vec3(0.0f, 0.0f, 3.0f);

    BoneAttachmentSystem::updateAttachments(world.registry());
    TransformSystem::updateWorldTransforms(world.registry());

    const auto* transform = world.registry().get<engine::ecs::Transform>(target);
    assert_close(transform->position.x, 4.0f, "Attachment should convert world target into parent local X");
    assert_close(transform->position.y, 2.0f, "Attachment should convert world target into parent local Y");
    assert_close(transform->position.z, 3.0f, "Attachment should convert world target into parent local Z");
    assert_close(transform->worldMatrix[3].x, 5.0f, "Attachment world X should follow source bone");
    assert_close(transform->worldMatrix[3].y, 2.0f, "Attachment world Y should follow source bone");
    assert_close(transform->worldMatrix[3].z, 3.0f, "Attachment world Z should include grip offset");
}

void test_bone_attachment_ignores_missing_source()
{
    World world;
    engine::ecs::EntityId source = world.createEntity("source");
    engine::ecs::EntityId target = world.createEntity("target", glm::vec3(3.0f, 0.0f, 0.0f));
    auto& attachment = world.registry().emplace<engine::ecs::BoneAttachment>(target);
    attachment.sourceEntity = source;
    attachment.boneName = "hand";

    world.destroyEntity(source);
    BoneAttachmentSystem::updateAttachments(world.registry());

    const auto* transform = world.registry().get<engine::ecs::Transform>(target);
    assert_close(transform->position.x, 3.0f, "Missing source should leave attachment target unchanged");
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
    run_test("hierarchy_rejects_cycles", test_hierarchy_rejects_cycles);
    run_test("world_factory_renderable_handles", test_world_factory_renderable_handles);
    run_test("world_recursive_destroy", test_world_recursive_destroy);
    run_test("collision_refreshes_world_transform", test_collision_refreshes_world_transform);
    run_test("collision_correction_respects_parent_space", test_collision_correction_respects_parent_space);
    run_test("sequential_collision_resolution_uses_fresh_transforms", test_sequential_collision_resolution_uses_fresh_transforms);
    run_test("bone_attachment_follows_parented_source_bone", test_bone_attachment_follows_parented_source_bone);
    run_test("bone_attachment_ignores_missing_source", test_bone_attachment_ignores_missing_source);
    run_test("system_manager_updates", test_system_manager_updates);

    std::cout << "Tests passed: " << tests_passed << std::endl;
    std::cout << "Tests failed: " << tests_failed << std::endl;
    return tests_failed == 0 ? 0 : 1;
}
