#include "Game/Entities/Crusader.hpp"
#include "Game/Entities/Enemy.hpp"
#include "Game/GameAnimationFactory.hpp"
#include "Game/GameEntityFactory.hpp"
#include "Game/GameSystems.hpp"
#include "Game/GameWeaponFactory.hpp"

#include "engine/assets/SkinnedMaterial.hpp"
#include "engine/assets/TintedMaterial.hpp"
#include "engine/components/Camera.hpp"
#include "engine/components/HealthComponent.hpp"
#include "engine/ecs/EcsComponents.hpp"
#include "engine/ecs/World.hpp"
#include "engine/systems/BoneAttachmentSystem.hpp"
#include "engine/systems/TransformSystem.hpp"

#include <glm/geometric.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

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

std::shared_ptr<ModelAsset> make_swordman_like_model()
{
    auto model = std::make_shared<ModelAsset>();
    auto skeleton = std::make_shared<Skeleton>();
    skeleton->add_bone("_rootJoint", -1, glm::mat4(1.0f));
    skeleton->add_bone("UnknownSoldier_Spine1_03", 0, glm::mat4(1.0f));
    skeleton->add_bone("UnknownSoldier_Spine2_04", 1, glm::mat4(1.0f));
    skeleton->add_bone("UnknownSoldier_RightShoulder_031", 2, glm::mat4(1.0f));
    skeleton->add_bone("UnknownSoldier_RightArm_032", 3, glm::mat4(1.0f));
    skeleton->add_bone("UnknownSoldier_RightForeArm_033", 4, glm::mat4(1.0f));
    skeleton->add_bone("UnknownSoldier_RightHand_034", 5, glm::mat4(1.0f));
    skeleton->add_bone("UnknownSoldier_LeftShoulder_08", 2, glm::mat4(1.0f));
    skeleton->add_bone("UnknownSoldier_LeftArm_09", 7, glm::mat4(1.0f));
    skeleton->add_bone("UnknownSoldier_LeftForeArm_010", 8, glm::mat4(1.0f));
    model->skins.push_back(SkinAsset{"swordman", skeleton, {}});

    auto base = std::make_shared<AnimationClip>("base", 1.0f, 1.0f);
    for (int boneId = 0; boneId < skeleton->get_bone_count(); ++boneId) {
        BoneAnimation bone;
        bone.bone_id = boneId;
        bone.position_keys.push_back({0.0f, glm::vec3(0.0f)});
        bone.position_keys.push_back({1.0f, glm::vec3(0.0f)});
        bone.rotation_keys.push_back({0.0f, glm::quat(1.0f, 0.0f, 0.0f, 0.0f)});
        bone.rotation_keys.push_back({1.0f, glm::quat(1.0f, 0.0f, 0.0f, 0.0f)});
        bone.scale_keys.push_back({0.0f, glm::vec3(1.0f)});
        bone.scale_keys.push_back({1.0f, glm::vec3(1.0f)});
        base->add_bone_animation(bone);
    }
    model->animations.push_back(base);
    return model;
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
    model->skins.push_back(SkinAsset{"test", std::make_shared<Skeleton>(), {}});
    auto skinnedMaterial = std::make_shared<SkinnedMaterial>();
    auto skinned = factory.createSkinnedRenderable("skinned", {}, skinnedMaterial, model, root);
    auto* renderable = world.registry().get<engine::ecs::SkinnedRenderable>(skinned);

    assert_true(renderable != nullptr, "Skinned factory should add SkinnedRenderable");
    assert_true(renderable->model == model, "Skinned factory should keep model handle");
    assert_true(renderable->material == skinnedMaterial, "Skinned factory should keep material handle");
}

void test_player_attack_animation_factory()
{
    auto model = make_swordman_like_model();
    const int index = game::ensurePlayerAttackAnimation(*model);
    const int reused = game::ensurePlayerAttackAnimation(*model);

    assert_true(index == 1, "Attack animation should be appended after the base clip");
    assert_true(reused == index, "Attack animation factory should not duplicate clips");
    assert_true(model->animations.size() == 2, "Model should contain base and attack clips");

    const auto& clip = model->animations[static_cast<size_t>(index)];
    assert_true(clip->get_name() == "player_sword_attack", "Attack clip should be named");
    assert_true(clip->get_duration() == 0.25f, "Attack clip duration should match attack timer");
    assert_true(
        clip->get_bone_animations().size() == static_cast<size_t>(model->skins.front().skeleton->get_bone_count()),
        "Attack clip should contain a channel for every bone");

    const int rightArm = model->skins.front().skeleton->get_bone_id("UnknownSoldier_RightArm_032");
    const BoneAnimation* rightArmAnimation = clip->get_bone_animation(rightArm);
    assert_true(rightArmAnimation != nullptr, "Attack clip should animate the right arm");
    assert_true(rightArmAnimation->rotation_keys.size() == 4, "Attack right arm should have four keyframes");

    const glm::quat& rest = rightArmAnimation->rotation_keys.front().rotation;
    const glm::quat& strike = rightArmAnimation->rotation_keys[2].rotation;
    const float rotationDelta =
        glm::abs(rest.x - strike.x) +
        glm::abs(rest.y - strike.y) +
        glm::abs(rest.z - strike.z) +
        glm::abs(rest.w - strike.w);
    assert_true(rotationDelta > 0.1f, "Attack clip should visibly rotate the sword arm");
}

void test_player_sword_factory_and_socket_selection()
{
    World world;
    engine::ecs::EntityId root = world.createEntity("root");
    game::SwordVisualAssets assets;
    assets.bladeRenderer = inert_renderer();
    assets.guardRenderer = inert_renderer();
    assets.gripRenderer = inert_renderer();
    assets.pommelRenderer = inert_renderer();
    assets.bladeMaterial = inert_material();
    assets.guardMaterial = inert_material();
    assets.gripMaterial = inert_material();

    engine::ecs::EntityId sword = game::createPlayerSword(world, root, assets);
    const auto* swordHierarchy = world.registry().get<engine::ecs::Hierarchy>(sword);

    assert_true(world.registry().isAlive(sword), "Sword factory should create a root entity");
    assert_true(swordHierarchy && swordHierarchy->children.size() == 4, "Sword should contain blade, guard, grip, and pommel");

    auto model = make_swordman_like_model();
    const Skeleton& skeleton = *model->skins.front().skeleton;
    assert_true(
        game::resolvePlayerSwordHandBoneName(skeleton) == game::kSwordmanRightHandBone,
        "Sword socket should fall back to the swordman right hand bone");

    engine::ecs::BoneAttachment attachment = game::makePlayerSwordAttachment(sword, skeleton);
    assert_true(attachment.boneName == game::kSwordmanRightHandBone, "Sword attachment should cache the resolved hand bone name");
    assert_true(attachment.boneId == skeleton.get_bone_id(game::kSwordmanRightHandBone), "Sword attachment should cache the hand bone id");

    auto locatorModel = make_swordman_like_model();
    Skeleton& locatorSkeleton = *locatorModel->skins.front().skeleton;
    const int handId = locatorSkeleton.get_bone_id(game::kSwordmanRightHandBone);
    const int locatorId = locatorSkeleton.add_bone(game::kSwordmanHandLocatorBone, handId, glm::mat4(1.0f));
    locatorSkeleton.get_bone(locatorId).local_transform =
        glm::scale(glm::mat4(1.0f), glm::vec3(0.05f));

    engine::ecs::BoneAttachment locatorAttachment = game::makePlayerSwordAttachment(sword, locatorSkeleton);
    assert_true(locatorAttachment.boneName == game::kSwordmanHandLocatorBone, "Sword socket should prefer the hand locator");
    assert_true(locatorAttachment.localScale.x > 1000.0f, "Locator scale should be compensated so the sword stays visible");
}

void test_player_sword_socket_calibration_faces_forward()
{
    World world;
    auto model = make_swordman_like_model();
    Skeleton& skeleton = *model->skins.front().skeleton;
    const int handId = skeleton.get_bone_id(game::kSwordmanRightHandBone);
    const int locatorId = skeleton.add_bone(game::kSwordmanHandLocatorBone, handId, glm::mat4(1.0f));
    skeleton.get_bone(locatorId).local_transform =
        glm::translate(glm::mat4(1.0f), glm::vec3(0.2f, 0.0f, 0.0f)) *
        glm::mat4_cast(glm::angleAxis(glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f))) *
        glm::scale(glm::mat4(1.0f), glm::vec3(0.05f));

    auto bindPose = std::make_shared<AnimationClip>("bind_pose", 1.0f, 1.0f);
    model->animations.push_back(bindPose);

    engine::ecs::EntityId visual = world.createEntity("visual");
    auto& animation = world.registry().emplace<engine::ecs::AnimatorData>(visual);
    animation.model = model;
    animation.skinIndex = 0;
    animation.animator.set_skeleton(model->skins.front().skeleton.get());
    animation.animator.play(bindPose.get(), true);

    engine::ecs::EntityId sword = world.createEntity("sword");
    auto attachment = game::makePlayerSwordAttachment(visual, skeleton);
    attachment.localOffset = glm::vec3(0.0f);
    attachment.localScale = glm::vec3(1.0f);
    world.registry().emplace<engine::ecs::BoneAttachment>(sword, attachment);

    BoneAttachmentSystem::updateAttachments(world.registry());
    TransformSystem::updateWorldTransforms(world.registry());

    const auto* transform = world.registry().get<engine::ecs::Transform>(sword);
    const glm::vec3 swordForward = glm::normalize(glm::mat3(transform->worldMatrix) * glm::vec3(0.0f, 0.0f, 1.0f));
    assert_true(
        glm::dot(swordForward, glm::vec3(0.0f, 0.0f, 1.0f)) > 0.99f,
        "Socket calibration should make the sword blade face player-forward");
}

void test_player_attack_points_sword_forward()
{
    World world;
    auto model = make_swordman_like_model();
    const int attackIndex = game::ensurePlayerAttackAnimation(*model);
    const Skeleton& skeleton = *model->skins.front().skeleton;

    engine::ecs::EntityId visual = world.createEntity("visual");
    auto& animation = world.registry().emplace<engine::ecs::AnimatorData>(visual);
    animation.model = model;
    animation.skinIndex = 0;
    animation.animator.set_skeleton(model->skins.front().skeleton.get());
    animation.animator.play(model->animations[static_cast<size_t>(attackIndex)].get(), false);
    animation.animator.set_current_time(0.16f);

    engine::ecs::EntityId sword = world.createEntity("sword");
    auto attachment = game::makePlayerSwordAttachment(visual, skeleton);
    attachment.localOffset = glm::vec3(0.0f);
    attachment.localScale = glm::vec3(1.0f);
    world.registry().emplace<engine::ecs::BoneAttachment>(sword, attachment);

    BoneAttachmentSystem::updateAttachments(world.registry());
    TransformSystem::updateWorldTransforms(world.registry());

    const auto* transform = world.registry().get<engine::ecs::Transform>(sword);
    const glm::vec3 swordForward = glm::normalize(glm::mat3(transform->worldMatrix) * glm::vec3(0.0f, 0.0f, 1.0f));
    assert_true(
        glm::dot(swordForward, glm::vec3(0.0f, 0.0f, 1.0f)) > 0.25f,
        "Attack contact frame should keep the sword facing generally forward");
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
    run_test("player_attack_animation_factory", test_player_attack_animation_factory);
    run_test("player_sword_factory_and_socket_selection", test_player_sword_factory_and_socket_selection);
    run_test("player_sword_socket_calibration_faces_forward", test_player_sword_socket_calibration_faces_forward);
    run_test("player_attack_points_sword_forward", test_player_attack_points_sword_forward);
    run_test("player_movement_attack_and_camera", test_player_movement_attack_and_camera);
    run_test("enemy_ai_moves_and_attacks", test_enemy_ai_moves_and_attacks);
    run_test("xp_level_up", test_xp_level_up);
    run_test("recursive_destroy_invalidates_children", test_recursive_destroy_invalidates_children);

    std::cout << "Tests passed: " << tests_passed << std::endl;
    std::cout << "Tests failed: " << tests_failed << std::endl;
    return tests_failed == 0 ? 0 : 1;
}
