#include "engine/assets/AssetManager.hpp"
#include "engine/assets/SkinnedMaterial.hpp"
#include "engine/ecs/EcsComponents.hpp"
#include "engine/ecs/Registry.hpp"
#include "engine/ecs/World.hpp"
#include "engine/systems/AnimationSystem.hpp"

#include <glm/gtc/matrix_transform.hpp>

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

std::shared_ptr<ModelAsset> make_test_model()
{
    auto model = std::make_shared<ModelAsset>();
    auto skeleton = std::make_shared<Skeleton>();
    skeleton->add_bone("root", -1, glm::mat4(1.0f));

    auto clip = std::make_shared<AnimationClip>("translate", 1.0f, 1.0f);
    BoneAnimation root;
    root.bone_id = 0;
    root.position_keys.push_back({0.0f, glm::vec3(0.0f)});
    root.position_keys.push_back({1.0f, glm::vec3(1.0f, 0.0f, 0.0f)});
    root.rotation_keys.push_back({0.0f, glm::quat(1.0f, 0.0f, 0.0f, 0.0f)});
    root.rotation_keys.push_back({1.0f, glm::quat(1.0f, 0.0f, 0.0f, 0.0f)});
    root.scale_keys.push_back({0.0f, glm::vec3(1.0f)});
    root.scale_keys.push_back({1.0f, glm::vec3(1.0f)});
    clip->add_bone_animation(root);
    model->skins.push_back(SkinAsset{"test", skeleton, {0}});
    model->animations.push_back(clip);

    return model;
}

void test_animator_updates_time_and_bones()
{
    auto model = make_test_model();
    Animator animator(model->skins.front().skeleton.get());
    animator.play(model->animations.front().get(), true);
    animator.update(0.25f);

    assert_true(animator.is_animation_playing(), "Animator should keep looping clip playing");
    assert_true(animator.get_current_time() > 0.0f, "Animator time should advance");
    assert_true(animator.get_bone_matrices().size() == 1, "Animator should produce one bone matrix");
}

void test_animator_exposes_model_space_bone_matrices()
{
    auto skeleton = std::make_shared<Skeleton>();
    skeleton->add_bone("root", -1, glm::mat4(1.0f));
    skeleton->add_bone("child", 0, glm::mat4(1.0f));

    auto clip = std::make_shared<AnimationClip>("child", 1.0f, 1.0f);
    BoneAnimation root;
    root.bone_id = 0;
    root.position_keys.push_back({0.0f, glm::vec3(1.0f, 0.0f, 0.0f)});
    root.rotation_keys.push_back({0.0f, glm::quat(1.0f, 0.0f, 0.0f, 0.0f)});
    root.scale_keys.push_back({0.0f, glm::vec3(1.0f)});
    clip->add_bone_animation(root);

    BoneAnimation child;
    child.bone_id = 1;
    child.position_keys.push_back({0.0f, glm::vec3(0.0f, 2.0f, 0.0f)});
    child.rotation_keys.push_back({0.0f, glm::quat(1.0f, 0.0f, 0.0f, 0.0f)});
    child.scale_keys.push_back({0.0f, glm::vec3(1.0f)});
    clip->add_bone_animation(child);

    Animator animator(skeleton.get());
    animator.play(clip.get(), true);

    const auto& boneModels = animator.get_bone_model_matrices();
    assert_true(boneModels.size() == 2, "Animator should expose one model matrix per bone");
    assert_true(animator.get_local_bone_transforms().size() == 2, "Animator should expose local bone transforms");
    assert_true(boneModels[1][3].x == 1.0f, "Child model matrix should include parent translation");
    assert_true(boneModels[1][3].y == 2.0f, "Child model matrix should include child translation");
}

void test_animation_system_updates_skinned_material()
{
    World world;
    auto model = make_test_model();
    auto material = std::make_shared<SkinnedMaterial>();

    engine::ecs::EntityId entity = world.createEntity("animated");
    world.registry().emplace<engine::ecs::SkinnedRenderable>(
        entity,
        engine::ecs::SkinnedRenderable{{}, material, model, 0});
    auto& animation = world.registry().emplace<engine::ecs::AnimatorData>(entity);
    animation.model = model;
    animation.skinIndex = 0;
    animation.currentAnimation = 0;
    animation.playing = true;
    animation.loop = true;

    AnimationSystem system;
    system.update(world.registry(), 0.25f);

    const auto* updated = world.registry().get<engine::ecs::AnimatorData>(entity);
    assert_true(updated != nullptr, "AnimatorData should remain attached");
    assert_true(updated->animator.get_current_time() > 0.0f, "AnimationSystem should advance animator time");
    assert_true(material->get_is_animated(), "AnimationSystem should mark material animated");
    assert_true(material->get_active_bone_count() == 1, "AnimationSystem should upload one active bone matrix");
}

void test_animation_system_uses_renderable_model_and_respects_non_loop_stop()
{
    World world;
    auto model = make_test_model();
    auto material = std::make_shared<SkinnedMaterial>();

    engine::ecs::EntityId entity = world.createEntity("animated");
    world.registry().emplace<engine::ecs::SkinnedRenderable>(
        entity,
        engine::ecs::SkinnedRenderable{{}, material, model, 0});
    auto& animation = world.registry().emplace<engine::ecs::AnimatorData>(entity);
    animation.currentAnimation = 0;
    animation.playing = true;
    animation.loop = false;

    AnimationSystem system;
    system.update(world.registry(), 2.0f);

    auto* updated = world.registry().get<engine::ecs::AnimatorData>(entity);
    assert_true(updated != nullptr, "AnimatorData should remain attached");
    assert_true(updated->model == model, "AnimationSystem should resolve model from SkinnedRenderable");
    assert_true(!updated->playing, "Non-looping animation should stop at the end");
    const float stoppedTime = updated->animator.get_current_time();

    system.update(world.registry(), 0.25f);
    assert_true(!updated->playing, "Stopped non-looping animation should not restart automatically");
    assert_true(
        updated->animator.get_current_time() == stoppedTime,
        "Stopped non-looping animation time should not advance after completion");
}

void test_animation_system_play_helper()
{
    World world;
    engine::ecs::EntityId entity = world.createEntity("animated");

    AnimationSystem::play(world.registry(), entity, 2, false, 1.5f);

    const auto* animation = world.registry().get<engine::ecs::AnimatorData>(entity);
    assert_true(animation != nullptr, "play helper should create AnimatorData");
    assert_true(animation->currentAnimation == 2, "play helper should set animation index");
    assert_true(!animation->loop, "play helper should set loop flag");
    assert_true(animation->speed == 1.5f, "play helper should set playback speed");
    assert_true(animation->playing, "play helper should mark animation playing");
}
} // namespace

int main()
{
    run_test("animator_updates_time_and_bones", test_animator_updates_time_and_bones);
    run_test("animator_exposes_model_space_bone_matrices", test_animator_exposes_model_space_bone_matrices);
    run_test("animation_system_updates_skinned_material", test_animation_system_updates_skinned_material);
    run_test("animation_system_uses_renderable_model_and_respects_non_loop_stop", test_animation_system_uses_renderable_model_and_respects_non_loop_stop);
    run_test("animation_system_play_helper", test_animation_system_play_helper);

    std::cout << "Tests passed: " << tests_passed << std::endl;
    std::cout << "Tests failed: " << tests_failed << std::endl;
    return tests_failed == 0 ? 0 : 1;
}
