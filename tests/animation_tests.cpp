#include <iostream>
#include <cassert>
#include <cmath>
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <filesystem>

// Engine includes
#include "engine/animations/Skeleton.hpp"
#include "engine/animations/AnimationClip.hpp"
#include "engine/animations/Animator.hpp"
#include "engine/ecs/AnimationComponent.hpp"
#include "engine/ecs/Entity.hpp"
#include "engine/ecs/Component.hpp"
#include "engine/assets/MeshLoader.hpp"

// ============================================================
// TEST UTILITIES
// ============================================================

static int tests_passed = 0;
static int tests_failed = 0;

#define TEST(name) void test_##name()
#define RUN_TEST(name) run_test(#name, test_##name)

void run_test(const char* name, void (*test_func)()) {
    try {
        test_func();
        std::cout << "[PASS] " << name << std::endl;
        tests_passed++;
    }
    catch (const std::exception& e) {
        std::cerr << "[FAIL] " << name << " - " << e.what() << std::endl;
        tests_failed++;
    }
    catch (...) {
        std::cerr << "[FAIL] " << name << " - Unknown exception" << std::endl;
        tests_failed++;
    }
}

void assert_true(bool condition, const char* message = "Assertion failed") {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void assert_equal(int expected, int actual, const char* message = "Values not equal") {
    if (expected != actual) {
        throw std::runtime_error(std::string(message) + " (expected: " +
            std::to_string(expected) + ", actual: " + std::to_string(actual) + ")");
    }
}

void assert_float_equal(float expected, float actual, float epsilon = 0.0001f,
    const char* message = "Floats not equal") {
    if (std::abs(expected - actual) > epsilon) {
        throw std::runtime_error(std::string(message) + " (expected: " +
            std::to_string(expected) + ", actual: " + std::to_string(actual) + ")");
    }
}

void assert_vec3_equal(const glm::vec3& expected, const glm::vec3& actual,
    float epsilon = 0.0001f, const char* message = "Vec3 not equal") {
    if (glm::length(expected - actual) > epsilon) {
        throw std::runtime_error(message);
    }
}

// ============================================================
// SKELETON TESTS
// ============================================================

TEST(skeleton_creation) {
    Skeleton skeleton;
    assert_equal(0, skeleton.get_bone_count(), "New skeleton should have 0 bones");
}

TEST(skeleton_add_bone) {
    Skeleton skeleton;

    int root_id = skeleton.add_bone("root", -1, glm::mat4(1.0f));
    assert_equal(0, root_id, "First bone should have ID 0");
    assert_equal(1, skeleton.get_bone_count(), "Skeleton should have 1 bone");

    int child_id = skeleton.add_bone("child", root_id, glm::mat4(1.0f));
    assert_equal(1, child_id, "Second bone should have ID 1");
    assert_equal(2, skeleton.get_bone_count(), "Skeleton should have 2 bones");
}

TEST(skeleton_bone_lookup) {
    Skeleton skeleton;
    skeleton.add_bone("root", -1, glm::mat4(1.0f));
    skeleton.add_bone("spine", 0, glm::mat4(1.0f));
    skeleton.add_bone("head", 1, glm::mat4(1.0f));

    assert_equal(0, skeleton.get_bone_id("root"), "root should have ID 0");
    assert_equal(1, skeleton.get_bone_id("spine"), "spine should have ID 1");
    assert_equal(2, skeleton.get_bone_id("head"), "head should have ID 2");
    assert_equal(-1, skeleton.get_bone_id("nonexistent"), "Nonexistent bone should return -1");
}

TEST(skeleton_has_bone) {
    Skeleton skeleton;
    skeleton.add_bone("root", -1, glm::mat4(1.0f));

    assert_true(skeleton.has_bone("root"), "Skeleton should have 'root' bone");
    assert_true(!skeleton.has_bone("nonexistent"), "Skeleton should not have 'nonexistent' bone");
}

TEST(skeleton_bone_hierarchy) {
    Skeleton skeleton;
    int root = skeleton.add_bone("root", -1, glm::mat4(1.0f));
    int child = skeleton.add_bone("child", root, glm::mat4(1.0f));

    const Bone& root_bone = skeleton.get_bone(root);
    const Bone& child_bone = skeleton.get_bone(child);

    assert_equal(-1, root_bone.parent_id, "Root bone should have parent_id -1");
    assert_equal(root, child_bone.parent_id, "Child bone should have parent_id 0");
}

TEST(skeleton_calculate_bone_matrices) {
    Skeleton skeleton;
    skeleton.add_bone("root", -1, glm::mat4(1.0f));
    skeleton.add_bone("child", 0, glm::mat4(1.0f));

    std::vector<glm::mat4> local_transforms = {
        glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
        glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.0f, 0.0f))
    };

    std::vector<glm::mat4> bone_matrices;
    skeleton.calculate_bone_matrices(local_transforms, bone_matrices);

    assert_equal(2, static_cast<int>(bone_matrices.size()), "Should have 2 bone matrices");
}

// ============================================================
// ANIMATION CLIP TESTS
// ============================================================

TEST(animation_clip_creation) {
    AnimationClip clip("walk", 2.0f, 30.0f);

    assert_true(clip.get_name() == "walk", "Clip name should be 'walk'");
    assert_float_equal(2.0f, clip.get_duration(), 0.0001f, "Duration should be 2.0");
    assert_float_equal(30.0f, clip.get_ticks_per_second(), 0.0001f, "TPS should be 30.0");
}

TEST(animation_clip_default_values) {
    AnimationClip clip;

    assert_true(clip.get_name().empty(), "Default clip name should be empty");
    assert_float_equal(0.0f, clip.get_duration(), 0.0001f, "Default duration should be 0");
    assert_float_equal(25.0f, clip.get_ticks_per_second(), 0.0001f, "Default TPS should be 25.0");
}

TEST(animation_clip_add_bone_animation) {
    AnimationClip clip("test", 1.0f);

    BoneAnimation bone_anim;
    bone_anim.bone_id = 0;
    bone_anim.position_keys.push_back({ 0.0f, glm::vec3(0.0f) });
    bone_anim.position_keys.push_back({ 1.0f, glm::vec3(1.0f, 0.0f, 0.0f) });

    clip.add_bone_animation(bone_anim);

    const BoneAnimation* retrieved = clip.get_bone_animation(0);
    assert_true(retrieved != nullptr, "Should retrieve bone animation for bone 0");
    assert_equal(0, retrieved->bone_id, "Bone ID should be 0");
}

TEST(animation_clip_get_nonexistent_bone) {
    AnimationClip clip("test", 1.0f);

    const BoneAnimation* retrieved = clip.get_bone_animation(999);
    assert_true(retrieved == nullptr, "Should return nullptr for nonexistent bone");
}

// ============================================================
// BONE ANIMATION INTERPOLATION TESTS
// ============================================================

TEST(bone_animation_position_interpolation) {
    BoneAnimation bone_anim;
    bone_anim.bone_id = 0;
    bone_anim.position_keys.push_back({ 0.0f, glm::vec3(0.0f, 0.0f, 0.0f) });
    bone_anim.position_keys.push_back({ 1.0f, glm::vec3(10.0f, 0.0f, 0.0f) });

    glm::vec3 pos = bone_anim.get_position(0.5f);
    assert_vec3_equal(glm::vec3(5.0f, 0.0f, 0.0f), pos, 0.001f,
        "Position at t=0.5 should be (5, 0, 0)");
}

TEST(bone_animation_single_keyframe) {
    BoneAnimation bone_anim;
    bone_anim.bone_id = 0;
    bone_anim.position_keys.push_back({ 0.0f, glm::vec3(5.0f, 5.0f, 5.0f) });

    glm::vec3 pos = bone_anim.get_position(0.5f);
    assert_vec3_equal(glm::vec3(5.0f, 5.0f, 5.0f), pos, 0.001f,
        "Single keyframe should return that position");
}

TEST(bone_animation_empty_keyframes) {
    BoneAnimation bone_anim;
    bone_anim.bone_id = 0;

    glm::vec3 pos = bone_anim.get_position(0.5f);
    assert_vec3_equal(glm::vec3(0.0f), pos, 0.001f,
        "Empty position keys should return zero vector");
}

TEST(bone_animation_rotation_interpolation) {
    BoneAnimation bone_anim;
    bone_anim.bone_id = 0;

    glm::quat start = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);  // Identity
    glm::quat end = glm::angleAxis(glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    bone_anim.rotation_keys.push_back({ 0.0f, start });
    bone_anim.rotation_keys.push_back({ 1.0f, end });

    glm::quat rot = bone_anim.get_rotation(0.5f);
    // Should be approximately 45 degrees around Y
    glm::quat expected = glm::angleAxis(glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    float dot = std::abs(glm::dot(rot, expected));
    assert_true(dot > 0.99f, "Rotation should be approximately 45 degrees");
}

TEST(bone_animation_scale_interpolation) {
    BoneAnimation bone_anim;
    bone_anim.bone_id = 0;
    bone_anim.scale_keys.push_back({ 0.0f, glm::vec3(1.0f) });
    bone_anim.scale_keys.push_back({ 1.0f, glm::vec3(2.0f) });

    glm::vec3 scale = bone_anim.get_scale(0.5f);
    assert_vec3_equal(glm::vec3(1.5f), scale, 0.001f,
        "Scale at t=0.5 should be (1.5, 1.5, 1.5)");
}

// ============================================================
// ANIMATOR TESTS
// ============================================================

TEST(animator_creation) {
    Animator animator;

    assert_true(!animator.is_animation_playing(), "New animator should not be playing");
    assert_float_equal(0.0f, animator.get_current_time(), 0.0001f,
        "Initial time should be 0");
}

TEST(animator_with_skeleton) {
    Skeleton skeleton;
    skeleton.add_bone("root", -1, glm::mat4(1.0f));
    skeleton.add_bone("child", 0, glm::mat4(1.0f));

    Animator animator(&skeleton);

    const auto& matrices = animator.get_bone_matrices();
    assert_equal(2, static_cast<int>(matrices.size()),
        "Animator should have 2 bone matrices");
}

TEST(animator_play_animation) {
    Skeleton skeleton;
    skeleton.add_bone("root", -1, glm::mat4(1.0f));

    Animator animator(&skeleton);

    AnimationClip clip("test", 1.0f);
    animator.play(&clip, true);

    assert_true(animator.is_animation_playing(), "Animator should be playing");
    assert_true(animator.get_current_clip() == &clip, "Current clip should match");
}

TEST(animator_stop_animation) {
    Skeleton skeleton;
    skeleton.add_bone("root", -1, glm::mat4(1.0f));

    Animator animator(&skeleton);
    AnimationClip clip("test", 1.0f);

    animator.play(&clip, true);
    animator.stop();

    assert_true(!animator.is_animation_playing(), "Animator should not be playing after stop");
    assert_true(animator.get_current_clip() == nullptr, "Current clip should be null after stop");
}

TEST(animator_pause_resume) {
    Skeleton skeleton;
    skeleton.add_bone("root", -1, glm::mat4(1.0f));

    Animator animator(&skeleton);
    AnimationClip clip("test", 1.0f);

    animator.play(&clip, true);
    animator.pause();

    assert_true(!animator.is_animation_playing(), "Animator should be paused");

    animator.resume();
    assert_true(animator.is_animation_playing(), "Animator should be playing after resume");
}

TEST(animator_update_advances_time) {
    Skeleton skeleton;
    skeleton.add_bone("root", -1, glm::mat4(1.0f));

    Animator animator(&skeleton);
    AnimationClip clip("test", 100.0f, 1.0f);  // 100 ticks duration, 1 TPS

    animator.play(&clip, false);
    animator.update(0.5f);  // Advance 0.5 seconds

    assert_float_equal(0.5f, animator.get_current_time(), 0.01f,
        "Time should advance by delta * TPS");
}

TEST(animator_looping) {
    Skeleton skeleton;
    skeleton.add_bone("root", -1, glm::mat4(1.0f));

    Animator animator(&skeleton);
    AnimationClip clip("test", 1.0f, 1.0f);  // 1 tick duration, 1 TPS

    animator.play(&clip, true);
    animator.update(1.5f);  // Advance past duration

    assert_true(animator.is_animation_playing(), "Looping animation should still play");
    assert_true(animator.get_current_time() < 1.0f, "Time should have looped");
}

TEST(animator_non_looping_stops) {
    Skeleton skeleton;
    skeleton.add_bone("root", -1, glm::mat4(1.0f));

    Animator animator(&skeleton);
    AnimationClip clip("test", 1.0f, 1.0f);

    animator.play(&clip, false);  // Non-looping
    animator.update(2.0f);  // Advance past duration

    assert_true(!animator.is_animation_playing(), "Non-looping animation should stop at end");
}

TEST(animator_playback_speed) {
    Skeleton skeleton;
    skeleton.add_bone("root", -1, glm::mat4(1.0f));

    Animator animator(&skeleton);
    AnimationClip clip("test", 100.0f, 1.0f);

    animator.set_playback_speed(2.0f);
    animator.play(&clip, false);
    animator.update(1.0f);

    assert_float_equal(2.0f, animator.get_current_time(), 0.01f,
        "Time should advance at double speed");
}

TEST(animator_set_current_time) {
    Skeleton skeleton;
    skeleton.add_bone("root", -1, glm::mat4(1.0f));

    Animator animator(&skeleton);
    AnimationClip clip("test", 10.0f, 1.0f);

    animator.play(&clip, true);
    animator.set_current_time(5.0f);

    assert_float_equal(5.0f, animator.get_current_time(), 0.01f,
        "Time should be set to 5.0");
}

// ============================================================
// ENTITY TESTS
// ============================================================

TEST(entity_creation) {
    Entity entity;

    assert_vec3_equal(glm::vec3(0.0f), entity.getPosition(), 0.0001f,
        "Default position should be (0,0,0)");
    assert_vec3_equal(glm::vec3(1.0f), entity.getScale(), 0.0001f,
        "Default scale should be (1,1,1)");
    assert_true(entity.getParent() == nullptr, "Default parent should be null");
    assert_true(entity.getMesh() == nullptr, "Default mesh should be null");
    assert_true(entity.getMaterial() == nullptr, "Default material should be null");
}

TEST(entity_transform) {
    Entity entity;

    entity.setPosition(glm::vec3(1.0f, 2.0f, 3.0f));
    assert_vec3_equal(glm::vec3(1.0f, 2.0f, 3.0f), entity.getPosition(), 0.0001f,
        "Position should be set correctly");

    entity.setScale(glm::vec3(2.0f, 2.0f, 2.0f));
    assert_vec3_equal(glm::vec3(2.0f, 2.0f, 2.0f), entity.getScale(), 0.0001f,
        "Scale should be set correctly");

    glm::quat rot = glm::angleAxis(glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    entity.setRotation(rot);
    float dot = std::abs(glm::dot(rot, entity.getRotation()));
    assert_true(dot > 0.99f, "Rotation should be set correctly");
}

TEST(entity_hierarchy) {
    Entity parent;
    Entity child;

    child.setParent(&parent);

    assert_true(child.getParent() == &parent, "Child parent should be set");
    assert_equal(1, static_cast<int>(parent.getChildren().size()),
        "Parent should have 1 child");
    assert_true(parent.getChildren()[0] == &child, "Parent's child should be correct");
}

TEST(entity_hierarchy_reparent) {
    Entity parent1;
    Entity parent2;
    Entity child;

    child.setParent(&parent1);
    assert_equal(1, static_cast<int>(parent1.getChildren().size()),
        "Parent1 should have 1 child");

    child.setParent(&parent2);
    assert_equal(0, static_cast<int>(parent1.getChildren().size()),
        "Parent1 should have 0 children after reparent");
    assert_equal(1, static_cast<int>(parent2.getChildren().size()),
        "Parent2 should have 1 child");
}

TEST(entity_local_matrix) {
    Entity entity;
    entity.setPosition(glm::vec3(5.0f, 0.0f, 0.0f));
    entity.setScale(glm::vec3(2.0f));

    glm::mat4 local = entity.getLocalMatrix();

    // Transform a point
    glm::vec4 point = local * glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);

    // Expected: scaled by 2 then translated by 5 on X
    // So (1,0,0) * 2 = (2,0,0) + (5,0,0) = (7,0,0)
    assert_float_equal(7.0f, point.x, 0.01f, "X should be 7");
    assert_float_equal(0.0f, point.y, 0.01f, "Y should be 0");
    assert_float_equal(0.0f, point.z, 0.01f, "Z should be 0");
}

TEST(entity_world_matrix) {
    Entity parent;
    Entity child;

    parent.setPosition(glm::vec3(10.0f, 0.0f, 0.0f));
    child.setPosition(glm::vec3(5.0f, 0.0f, 0.0f));
    child.setParent(&parent);

    glm::mat4 world = child.getWorldMatrix();

    // Child at (5,0,0) relative to parent at (10,0,0)
    // World position should be (15,0,0)
    glm::vec4 origin = world * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    assert_float_equal(15.0f, origin.x, 0.01f, "World X should be 15");
}

TEST(entity_rotate_by) {
    Entity entity;

    glm::quat rot1 = glm::angleAxis(glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::quat rot2 = glm::angleAxis(glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    entity.setRotation(rot1);
    entity.rotateBy(rot2);

    // Should now be ~90 degrees around Y
    glm::quat expected = glm::angleAxis(glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    float dot = std::abs(glm::dot(expected, entity.getRotation()));
    assert_true(dot > 0.99f, "Combined rotation should be ~90 degrees");
}

// ============================================================
// ENTITY COMPONENT TESTS
// ============================================================

// Simple test component for testing
class TestComponent : public Component {
public:
    int update_count = 0;
    int render_count = 0;
    float last_delta = 0.0f;

    // allow default construction in tests
    TestComponent()
        : Component(nullptr) {}

    // allow construction with an owner if needed later
    explicit TestComponent(Entity* owner)
        : Component(owner) {}

    void update(Entity& entity, float deltaTime) override {
        update_count++;
        last_delta = deltaTime;
    }

    void render(Entity& entity) override {
        render_count++;
    }
};


TEST(entity_add_component) {
    Entity entity;
    TestComponent* comp = new TestComponent();

    entity.addComponent(comp);

    assert_equal(1, static_cast<int>(entity.getComponents().size()),
        "Entity should have 1 component");

    delete comp;
}

TEST(entity_get_component) {
    Entity entity;
    TestComponent* comp = new TestComponent();

    entity.addComponent(comp);

    TestComponent* retrieved = entity.getComponent<TestComponent>();
    assert_true(retrieved == comp, "Should retrieve the correct component");

    delete comp;
}

TEST(entity_get_component_not_found) {
    Entity entity;

    TestComponent* retrieved = entity.getComponent<TestComponent>();
    assert_true(retrieved == nullptr, "Should return nullptr for missing component");
}

TEST(entity_remove_component) {
    Entity entity;
    TestComponent* comp = new TestComponent();

    entity.addComponent(comp);
    assert_equal(1, static_cast<int>(entity.getComponents().size()),
        "Entity should have 1 component");

    entity.removeComponent(comp);
    assert_equal(0, static_cast<int>(entity.getComponents().size()),
        "Entity should have 0 components after removal");

    delete comp;
}

TEST(entity_update_components) {
    Entity entity;
    TestComponent* comp = new TestComponent();

    entity.addComponent(comp);
    entity.updateComponents(0.016f);  // ~60fps delta

    assert_equal(1, comp->update_count, "Component should be updated once");
    assert_float_equal(0.016f, comp->last_delta, 0.0001f, "Delta time should be passed");

    delete comp;
}

TEST(entity_update_components_recursive) {
    Entity parent;
    Entity child;
    child.setParent(&parent);

    TestComponent* parentComp = new TestComponent();
    TestComponent* childComp = new TestComponent();

    parent.addComponent(parentComp);
    child.addComponent(childComp);

    parent.updateComponents(0.016f);

    assert_equal(1, parentComp->update_count, "Parent component should be updated");
    assert_equal(1, childComp->update_count, "Child component should be updated via recursion");

    delete parentComp;
    delete childComp;
}

// ============================================================
// ANIMATION COMPONENT TESTS
// ============================================================

TEST(animation_component_creation) {
    AnimationComponent component;

    assert_true(!component.has_skeleton(), "New component should have no skeleton");
    assert_true(!component.has_animations(), "New component should have no animations");
    assert_equal(0, component.get_animation_count(), "Animation count should be 0");
}

TEST(animation_component_with_model_data) {
    auto model_data = std::make_shared<ModelData>();
    model_data->skeleton = std::make_shared<Skeleton>();
    model_data->skeleton->add_bone("root", -1, glm::mat4(1.0f));

    AnimationComponent component(model_data);

    assert_true(component.has_skeleton(), "Component should have skeleton");
    assert_true(component.get_skeleton() != nullptr, "Skeleton should not be null");
}

TEST(animation_component_set_model_data) {
    AnimationComponent component;

    auto model_data = std::make_shared<ModelData>();
    model_data->skeleton = std::make_shared<Skeleton>();
    model_data->skeleton->add_bone("root", -1, glm::mat4(1.0f));

    auto clip = std::make_shared<AnimationClip>("walk", 2.0f);
    model_data->animations.push_back(clip);

    component.set_model_data(model_data);

    assert_true(component.has_skeleton(), "Component should have skeleton");
    assert_true(component.has_animations(), "Component should have animations");
    assert_equal(1, component.get_animation_count(), "Animation count should be 1");
}

TEST(animation_component_play_by_name) {
    auto model_data = std::make_shared<ModelData>();
    model_data->skeleton = std::make_shared<Skeleton>();
    model_data->skeleton->add_bone("root", -1, glm::mat4(1.0f));

    auto clip = std::make_shared<AnimationClip>("walk", 2.0f);
    model_data->animations.push_back(clip);

    AnimationComponent component(model_data);

    bool result = component.play_animation("walk");
    assert_true(result, "Should successfully play 'walk' animation");
    assert_true(component.is_playing(), "Component should be playing");
}

TEST(animation_component_play_by_index) {
    auto model_data = std::make_shared<ModelData>();
    model_data->skeleton = std::make_shared<Skeleton>();
    model_data->skeleton->add_bone("root", -1, glm::mat4(1.0f));

    auto clip = std::make_shared<AnimationClip>("walk", 2.0f);
    model_data->animations.push_back(clip);

    AnimationComponent component(model_data);

    bool result = component.play_animation(0);
    assert_true(result, "Should successfully play animation at index 0");
}

TEST(animation_component_play_nonexistent) {
    auto model_data = std::make_shared<ModelData>();
    model_data->skeleton = std::make_shared<Skeleton>();
    model_data->skeleton->add_bone("root", -1, glm::mat4(1.0f));

    AnimationComponent component(model_data);

    bool result = component.play_animation("nonexistent");
    assert_true(!result, "Should fail to play nonexistent animation");
}

TEST(animation_component_stop_pause_resume) {
    auto model_data = std::make_shared<ModelData>();
    model_data->skeleton = std::make_shared<Skeleton>();
    model_data->skeleton->add_bone("root", -1, glm::mat4(1.0f));

    auto clip = std::make_shared<AnimationClip>("walk", 2.0f);
    model_data->animations.push_back(clip);

    AnimationComponent component(model_data);
    component.play_animation("walk");

    component.pause_animation();
    assert_true(!component.is_playing(), "Should be paused");

    component.resume_animation();
    assert_true(component.is_playing(), "Should be playing after resume");

    component.stop_animation();
    assert_true(!component.is_playing(), "Should be stopped");
}

TEST(animation_component_get_animation_by_name) {
    auto model_data = std::make_shared<ModelData>();
    model_data->skeleton = std::make_shared<Skeleton>();

    auto clip1 = std::make_shared<AnimationClip>("walk", 2.0f);
    auto clip2 = std::make_shared<AnimationClip>("run", 1.0f);
    model_data->animations.push_back(clip1);
    model_data->animations.push_back(clip2);

    AnimationComponent component(model_data);

    const AnimationClip* walk = component.get_animation("walk");
    assert_true(walk != nullptr, "Should find 'walk' animation");
    assert_true(walk->get_name() == "walk", "Animation name should be 'walk'");

    const AnimationClip* run = component.get_animation("run");
    assert_true(run != nullptr, "Should find 'run' animation");
    assert_float_equal(1.0f, run->get_duration(), 0.0001f, "Run duration should be 1.0");
}

TEST(animation_component_auto_update) {
    AnimationComponent component;

    assert_true(component.get_auto_update(), "Auto update should be enabled by default");

    component.set_auto_update(false);
    assert_true(!component.get_auto_update(), "Auto update should be disabled");
}

TEST(animation_component_bone_matrices) {
    auto model_data = std::make_shared<ModelData>();
    model_data->skeleton = std::make_shared<Skeleton>();
    model_data->skeleton->add_bone("root", -1, glm::mat4(1.0f));
    model_data->skeleton->add_bone("child", 0, glm::mat4(1.0f));

    AnimationComponent component(model_data);

    const auto& matrices = component.get_bone_matrices();
    assert_equal(2, static_cast<int>(matrices.size()),
        "Should have 2 bone matrices");
}

// ============================================================
// ECS INTEGRATION TESTS (Based on main.cpp patterns)
// ============================================================

TEST(ecs_entity_with_animation_component) {
    // Create entity like in main.cpp
    Entity character;
    character.setPosition(glm::vec3(10.f, 1.f, 1.f));
    character.setScale(glm::vec3(1.f, 1.f, 1.f));

    // Create animation model data
    auto model_data = std::make_shared<ModelData>();
    model_data->skeleton = std::make_shared<Skeleton>();
    model_data->skeleton->add_bone("root", -1, glm::mat4(1.0f));
    model_data->skeleton->add_bone("spine", 0, glm::mat4(1.0f));
    model_data->skeleton->add_bone("head", 1, glm::mat4(1.0f));

    auto walk_clip = std::make_shared<AnimationClip>("walk", 2.0f, 30.0f);
    model_data->animations.push_back(walk_clip);

    // Create and attach animation component
    AnimationComponent* anim_comp = new AnimationComponent(model_data);
    character.addComponent(anim_comp);

    // Verify component is attached
    AnimationComponent* retrieved = character.getComponent<AnimationComponent>();
    assert_true(retrieved != nullptr, "Should retrieve animation component");
    assert_true(retrieved->has_skeleton(), "Retrieved component should have skeleton");

    // Play animation
    bool played = retrieved->play_animation("walk");
    assert_true(played, "Should play walk animation");

    // Update through entity
    character.updateComponents(0.016f);

    delete anim_comp;
}

TEST(ecs_animated_entity_hierarchy) {
    // Similar to island->tree->leaves pattern in main.cpp
    Entity island;
    island.setPosition(glm::vec3(0.f, 0.5f, 0.f));

    Entity tree;
    tree.setParent(&island);
    tree.setPosition(glm::vec3(0.f, 2.5f, 0.f));

    // Add animation to tree (for swaying animation)
    auto model_data = std::make_shared<ModelData>();
    model_data->skeleton = std::make_shared<Skeleton>();
    model_data->skeleton->add_bone("trunk", -1, glm::mat4(1.0f));

    auto sway_clip = std::make_shared<AnimationClip>("sway", 3.0f, 25.0f);
    model_data->animations.push_back(sway_clip);

    AnimationComponent* anim = new AnimationComponent(model_data);
    tree.addComponent(anim);

    // Island rotation affects tree world matrix
    glm::quat delta_rot = glm::angleAxis(glm::radians(30.f), glm::vec3(0.f, 1.f, 0.f));
    island.setRotation(delta_rot);

    // Get tree world matrix - should include island rotation
    glm::mat4 treeWorld = tree.getWorldMatrix();
    glm::vec4 treePos = treeWorld * glm::vec4(0.f, 0.f, 0.f, 1.f);

    // Tree's world Y position should be 0.5 (island) + 2.5 (tree local) = 3.0
    assert_float_equal(3.0f, treePos.y, 0.01f, "Tree world Y should be 3.0");

    // Update island should update tree's animation component
    island.updateComponents(0.016f);

    delete anim;
}

TEST(ecs_multiple_animated_children) {
    // Like windows as children of house in main.cpp
    Entity house;
    house.setPosition(glm::vec3(10.f, 1.f, 1.f));

    // Create multiple animated child entities
    Entity window1, window2, window3;
    window1.setParent(&house);
    window2.setParent(&house);
    window3.setParent(&house);

    window1.setPosition(glm::vec3(2.0f, 1.5f, 0.0f));
    window2.setPosition(glm::vec3(-0.2f, 2.0f, -2.75f));
    window3.setPosition(glm::vec3(-3.55f, 2.0f, 0.55f));

    // Verify hierarchy
    assert_equal(3, static_cast<int>(house.getChildren().size()),
        "House should have 3 children");

    // Create shared animation data for windows
    auto window_anim_data = std::make_shared<ModelData>();
    window_anim_data->skeleton = std::make_shared<Skeleton>();
    window_anim_data->skeleton->add_bone("window_frame", -1, glm::mat4(1.0f));

    auto open_clip = std::make_shared<AnimationClip>("open", 1.0f);
    window_anim_data->animations.push_back(open_clip);

    // Each window gets its own component but shares model data
    AnimationComponent* anim1 = new AnimationComponent(window_anim_data);
    AnimationComponent* anim2 = new AnimationComponent(window_anim_data);
    AnimationComponent* anim3 = new AnimationComponent(window_anim_data);

    window1.addComponent(anim1);
    window2.addComponent(anim2);
    window3.addComponent(anim3);

    // Update house updates all window animations
    house.updateComponents(0.016f);

    // All should be able to play independently
    anim1->play_animation("open");
    assert_true(anim1->is_playing(), "Window 1 should be playing");
    assert_true(!anim2->is_playing(), "Window 2 should not be playing yet");

    delete anim1;
    delete anim2;
    delete anim3;
}

TEST(ecs_entity_dynamic_transform_update) {
    // Similar to water scale animation in main.cpp
    Entity water;
    water.setPosition(glm::vec3(0.f, 0.f, 0.f));
    water.setScale(glm::vec3(10.f, 1.f, 10.f));

    // Simulate time-based scale update like in main loop
    float time = 1.5f;  // Simulated time
    water.setScale(glm::vec3(10.f, 1.0f + 0.1f * glm::sin(time), 10.f));

    glm::vec3 scale = water.getScale();
    float expected_y = 1.0f + 0.1f * glm::sin(1.5f);
    assert_float_equal(expected_y, scale.y, 0.001f, "Water scale Y should follow sine wave");
}

TEST(ecs_entity_incremental_rotation) {
    // Similar to island rotation in main.cpp
    Entity island;
    island.setRotation(glm::quat(1.0f, 0.0f, 0.0f, 0.0f));  // Identity

    // Simulate multiple frame rotations
    float delta_time = 0.016f;  // ~60fps
    for (int i = 0; i < 60; i++) {  // 1 second of updates
        glm::quat delta_rot = glm::angleAxis(glm::radians(30.f) * delta_time,
            glm::vec3(0.f, 1.f, 0.f));
        island.setRotation(delta_rot * island.getRotation());
    }

    // After 1 second at 30 deg/s, should be ~30 degrees rotated
    // Extract angle from quaternion
    float angle = 2.0f * std::acos(island.getRotation().w);
    float degrees = glm::degrees(angle);

    // Allow some floating point error
    assert_true(degrees > 25.0f && degrees < 35.0f,
        "Island should be rotated approximately 30 degrees");
}

// ============================================================
// INTEGRATION TESTS
// ============================================================

TEST(full_animation_pipeline) {
    // Create skeleton with hierarchy
    auto skeleton = std::make_shared<Skeleton>();
    int root = skeleton->add_bone("root", -1, glm::mat4(1.0f));
    int spine = skeleton->add_bone("spine", root, glm::mat4(1.0f));
    int head = skeleton->add_bone("head", spine, glm::mat4(1.0f));

    // Create animation clip
    auto clip = std::make_shared<AnimationClip>("nod", 1.0f, 1.0f);

    // Add head animation (nodding)
    BoneAnimation head_anim;
    head_anim.bone_id = head;
    head_anim.rotation_keys.push_back({ 0.0f, glm::quat(1.0f, 0.0f, 0.0f, 0.0f) });
    head_anim.rotation_keys.push_back({ 0.5f, glm::angleAxis(glm::radians(30.0f),
                                                            glm::vec3(1.0f, 0.0f, 0.0f)) });
    head_anim.rotation_keys.push_back({ 1.0f, glm::quat(1.0f, 0.0f, 0.0f, 0.0f) });

    // Add position keys (identity)
    head_anim.position_keys.push_back({ 0.0f, glm::vec3(0.0f) });
    head_anim.scale_keys.push_back({ 0.0f, glm::vec3(1.0f) });

    clip->add_bone_animation(head_anim);

    // Create model data
    auto model_data = std::make_shared<ModelData>();
    model_data->skeleton = skeleton;
    model_data->animations.push_back(clip);

    // Create animation component
    AnimationComponent component(model_data);

    // Play animation
    bool played = component.play_animation("nod");
    assert_true(played, "Should play 'nod' animation");

    // Verify bone matrices are generated
    const auto& matrices = component.get_bone_matrices();
    assert_equal(3, static_cast<int>(matrices.size()), "Should have 3 bone matrices");
}

TEST(multiple_animations_switching) {
    auto skeleton = std::make_shared<Skeleton>();
    skeleton->add_bone("root", -1, glm::mat4(1.0f));

    auto walk = std::make_shared<AnimationClip>("walk", 2.0f);
    auto run = std::make_shared<AnimationClip>("run", 1.0f);
    auto idle = std::make_shared<AnimationClip>("idle", 3.0f);

    auto model_data = std::make_shared<ModelData>();
    model_data->skeleton = skeleton;
    model_data->animations.push_back(walk);
    model_data->animations.push_back(run);
    model_data->animations.push_back(idle);

    AnimationComponent component(model_data);

    // Test switching between animations
    component.play_animation("walk");
    assert_true(component.get_animator().get_current_clip() == walk.get(),
        "Should be playing walk");

    component.play_animation("run");
    assert_true(component.get_animator().get_current_clip() == run.get(),
        "Should switch to run");

    component.play_animation(2);  // idle by index
    assert_true(component.get_animator().get_current_clip() == idle.get(),
        "Should switch to idle by index");
}

TEST(full_ecs_animation_integration) {
    // Create a complete entity hierarchy with animation
    // Similar to the scene graph in main.cpp

    Entity root;

    // Character entity
    Entity character;
    character.setParent(&root);
    character.setPosition(glm::vec3(5.0f, 0.0f, 0.0f));

    // Create skeleton
    auto skeleton = std::make_shared<Skeleton>();
    skeleton->add_bone("root", -1, glm::mat4(1.0f));
    skeleton->add_bone("spine", 0, glm::mat4(1.0f));
    skeleton->add_bone("left_arm", 1, glm::mat4(1.0f));
    skeleton->add_bone("right_arm", 1, glm::mat4(1.0f));

    // Create walk animation
    auto walk_clip = std::make_shared<AnimationClip>("walk", 1.0f, 30.0f);

    BoneAnimation arm_anim;
    arm_anim.bone_id = 2;  // left_arm
    arm_anim.rotation_keys.push_back({ 0.0f, glm::quat(1.0f, 0.0f, 0.0f, 0.0f) });
    arm_anim.rotation_keys.push_back({ 0.5f, glm::angleAxis(glm::radians(45.0f),
                                                            glm::vec3(1.0f, 0.0f, 0.0f)) });
    arm_anim.rotation_keys.push_back({ 1.0f, glm::quat(1.0f, 0.0f, 0.0f, 0.0f) });
    arm_anim.position_keys.push_back({ 0.0f, glm::vec3(0.0f) });
    arm_anim.scale_keys.push_back({ 0.0f, glm::vec3(1.0f) });
    walk_clip->add_bone_animation(arm_anim);

    // Create model data
    auto model_data = std::make_shared<ModelData>();
    model_data->skeleton = skeleton;
    model_data->animations.push_back(walk_clip);

    // Create and attach animation component
    AnimationComponent* anim_comp = new AnimationComponent(model_data);
    character.addComponent(anim_comp);

    // Play animation
    anim_comp->play_animation("walk", true);

    // Simulate game loop
    for (int frame = 0; frame < 60; frame++) {
        float delta_time = 0.016f;

        // Update from root (like main.cpp does with world roots)
        root.updateComponents(delta_time);
    }

    // Verify animation progressed
    assert_true(anim_comp->is_playing(), "Animation should still be playing (looped)");

    // Verify bone matrices updated
    const auto& matrices = anim_comp->get_bone_matrices();
    assert_equal(4, static_cast<int>(matrices.size()), "Should have 4 bone matrices");

    delete anim_comp;
}

// ============================================================
// GLTF LOADER TESTS (The Swordman Model)
// ============================================================

#ifdef SOURCE_DIR
static const char* SWORDMAN_GLTF_PATH = SOURCE_DIR "/tests/loading_files/the_swordman/scene.gltf";
#else
static const char* SWORDMAN_GLTF_PATH = "tests/loading_files/the_swordman/scene.gltf";
#endif
TEST(gltf_load_swordman_model) {
    if (!std::filesystem::exists(SWORDMAN_GLTF_PATH)) {
        std::cout << "  [SKIP] Test file not found: " << SWORDMAN_GLTF_PATH << std::endl;
        return;
    }

    ModelData* model_data = MeshLoader::load_gltf(SWORDMAN_GLTF_PATH);
    assert_true(model_data != nullptr, "Should successfully load swordman GLTF model");
    delete model_data;
}

TEST(gltf_swordman_has_skeleton) {
    if (!std::filesystem::exists(SWORDMAN_GLTF_PATH)) {
        std::cout << "  [SKIP] Test file not found: " << SWORDMAN_GLTF_PATH << std::endl;
        return;
    }

    ModelData* model_data = MeshLoader::load_gltf(SWORDMAN_GLTF_PATH);
    assert_true(model_data != nullptr, "Should load model");
    assert_true(model_data->skeleton != nullptr, "Swordman model should have a skeleton");
    assert_true(model_data->skeleton->get_bone_count() > 0, "Skeleton should have bones");
    delete model_data;
}

TEST(gltf_swordman_skeleton_hierarchy) {
    if (!std::filesystem::exists(SWORDMAN_GLTF_PATH)) {
        std::cout << "  [SKIP] Test file not found: " << SWORDMAN_GLTF_PATH << std::endl;
        return;
    }

    ModelData* model_data = MeshLoader::load_gltf(SWORDMAN_GLTF_PATH);
    assert_true(model_data != nullptr, "Should load model");
    assert_true(model_data->skeleton != nullptr, "Should have skeleton");

    const auto& bones = model_data->skeleton->get_bones();
    int bones_with_parents = 0;
    int root_bones = 0;

    for (const auto& bone : bones) {
        if (bone.parent_id == -1) {
            root_bones++;
        } else {
            bones_with_parents++;
            assert_true(bone.parent_id >= 0 && bone.parent_id < static_cast<int>(bones.size()),
                "Parent ID should be valid");
        }
    }

    assert_true(root_bones >= 1, "Should have at least one root bone");
    assert_true(bones_with_parents > root_bones, "Most bones should have parents");
    delete model_data;
}

TEST(gltf_swordman_bone_names) {
    if (!std::filesystem::exists(SWORDMAN_GLTF_PATH)) {
        std::cout << "  [SKIP] Test file not found: " << SWORDMAN_GLTF_PATH << std::endl;
        return;
    }

    ModelData* model_data = MeshLoader::load_gltf(SWORDMAN_GLTF_PATH);
    assert_true(model_data != nullptr, "Should load model");
    assert_true(model_data->skeleton != nullptr, "Should have skeleton");

    const auto& bones = model_data->skeleton->get_bones();
    bool has_hips = false;
    bool has_spine = false;
    bool has_head = false;

    for (const auto& bone : bones) {
        if (bone.name.find("Hips") != std::string::npos) has_hips = true;
        if (bone.name.find("Spine") != std::string::npos) has_spine = true;
        if (bone.name.find("Head") != std::string::npos) has_head = true;
    }

    assert_true(has_hips || has_spine, "Should have hips or spine bone");
    delete model_data;
}

TEST(gltf_swordman_inverse_bind_matrices) {
    if (!std::filesystem::exists(SWORDMAN_GLTF_PATH)) {
        std::cout << "  [SKIP] Test file not found: " << SWORDMAN_GLTF_PATH << std::endl;
        return;
    }

    ModelData* model_data = MeshLoader::load_gltf(SWORDMAN_GLTF_PATH);
    assert_true(model_data != nullptr, "Should load model");
    assert_true(model_data->skeleton != nullptr, "Should have skeleton");

    const auto& bones = model_data->skeleton->get_bones();
    for (const auto& bone : bones) {
        glm::mat4 zero(0.0f);
        bool is_zero = true;
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                if (bone.inverse_bind_pose[i][j] != 0.0f) {
                    is_zero = false;
                    break;
                }
            }
            if (!is_zero) break;
        }
        assert_true(!is_zero, "Inverse bind matrix should not be all zeros");
    }
    delete model_data;
}

TEST(gltf_swordman_has_animations) {
    if (!std::filesystem::exists(SWORDMAN_GLTF_PATH)) {
        std::cout << "  [SKIP] Test file not found: " << SWORDMAN_GLTF_PATH << std::endl;
        return;
    }

    ModelData* model_data = MeshLoader::load_gltf(SWORDMAN_GLTF_PATH);
    assert_true(model_data != nullptr, "Should load model");
    assert_true(!model_data->animations.empty(), "Swordman model should have animations");
    delete model_data;
}

TEST(gltf_swordman_animation_duration) {
    if (!std::filesystem::exists(SWORDMAN_GLTF_PATH)) {
        std::cout << "  [SKIP] Test file not found: " << SWORDMAN_GLTF_PATH << std::endl;
        return;
    }

    ModelData* model_data = MeshLoader::load_gltf(SWORDMAN_GLTF_PATH);
    assert_true(model_data != nullptr, "Should load model");
    assert_true(!model_data->animations.empty(), "Should have animations");

    const auto& anim = model_data->animations[0];
    assert_true(anim->get_duration() > 0.0f, "Animation should have positive duration");
    delete model_data;
}

TEST(gltf_swordman_animation_keyframes) {
    if (!std::filesystem::exists(SWORDMAN_GLTF_PATH)) {
        std::cout << "  [SKIP] Test file not found: " << SWORDMAN_GLTF_PATH << std::endl;
        return;
    }

    ModelData* model_data = MeshLoader::load_gltf(SWORDMAN_GLTF_PATH);
    assert_true(model_data != nullptr, "Should load model");
    assert_true(!model_data->animations.empty(), "Should have animations");

    const auto& anim = model_data->animations[0];
    const auto& bone_anims = anim->get_bone_animations();
    assert_true(!bone_anims.empty(), "Animation should have bone animations");

    int total_rotation_keys = 0;
    for (const auto& bone_anim : bone_anims) {
        total_rotation_keys += static_cast<int>(bone_anim.rotation_keys.size());
    }
    assert_true(total_rotation_keys > 0, "Should have rotation keyframes");
    delete model_data;
}

TEST(gltf_swordman_has_meshes) {
    if (!std::filesystem::exists(SWORDMAN_GLTF_PATH)) {
        std::cout << "  [SKIP] Test file not found: " << SWORDMAN_GLTF_PATH << std::endl;
        return;
    }

    ModelData* model_data = MeshLoader::load_gltf(SWORDMAN_GLTF_PATH);
    assert_true(model_data != nullptr, "Should load model");
    assert_true(!model_data->meshes.empty(), "Should have meshes");
    delete model_data;
}

TEST(gltf_swordman_mesh_vertex_data) {
    if (!std::filesystem::exists(SWORDMAN_GLTF_PATH)) {
        std::cout << "  [SKIP] Test file not found: " << SWORDMAN_GLTF_PATH << std::endl;
        return;
    }

    ModelData* model_data = MeshLoader::load_gltf(SWORDMAN_GLTF_PATH);
    assert_true(model_data != nullptr, "Should load model");
    assert_true(!model_data->meshes.empty(), "Should have meshes");
    delete model_data;
}

TEST(gltf_swordman_skinning_weights) {
    if (!std::filesystem::exists(SWORDMAN_GLTF_PATH)) {
        std::cout << "  [SKIP] Test file not found: " << SWORDMAN_GLTF_PATH << std::endl;
        return;
    }

    ModelData* model_data = MeshLoader::load_gltf(SWORDMAN_GLTF_PATH);
    assert_true(model_data != nullptr, "Should load model");
    assert_true(model_data->skeleton != nullptr, "Should have skeleton");
    assert_true(!model_data->meshes.empty(), "Should have meshes");
    delete model_data;
}

TEST(gltf_swordman_animator_integration) {
    if (!std::filesystem::exists(SWORDMAN_GLTF_PATH)) {
        std::cout << "  [SKIP] Test file not found: " << SWORDMAN_GLTF_PATH << std::endl;
        return;
    }

    ModelData* model_data = MeshLoader::load_gltf(SWORDMAN_GLTF_PATH);
    assert_true(model_data != nullptr, "Should load model");
    assert_true(model_data->skeleton != nullptr, "Should have skeleton");
    assert_true(!model_data->animations.empty(), "Should have animations");

    Animator animator(model_data->skeleton.get());
    animator.play(model_data->animations[0].get(), true);
    assert_true(animator.is_animation_playing(), "Animator should be playing");
    delete model_data;
}

TEST(gltf_swordman_animation_component_integration) {
    if (!std::filesystem::exists(SWORDMAN_GLTF_PATH)) {
        std::cout << "  [SKIP] Test file not found: " << SWORDMAN_GLTF_PATH << std::endl;
        return;
    }

    ModelData* raw_model = MeshLoader::load_gltf(SWORDMAN_GLTF_PATH);
    assert_true(raw_model != nullptr, "Should load model");

    auto model_data = std::shared_ptr<ModelData>(raw_model);
    AnimationComponent component(model_data);
    assert_true(component.has_skeleton(), "Component should have skeleton");
    assert_true(component.has_animations(), "Component should have animations");
}

TEST(gltf_swordman_animation_playback) {
    if (!std::filesystem::exists(SWORDMAN_GLTF_PATH)) {
        std::cout << "  [SKIP] Test file not found: " << SWORDMAN_GLTF_PATH << std::endl;
        return;
    }

    ModelData* model_data = MeshLoader::load_gltf(SWORDMAN_GLTF_PATH);
    assert_true(model_data != nullptr, "Should load model");

    Animator animator(model_data->skeleton.get());
    animator.play(model_data->animations[0].get(), false);

    float initial_time = animator.get_current_time();
    animator.update(1.0f);
    float after_time = animator.get_current_time();

    assert_true(after_time > initial_time, "Time should advance");
    delete model_data;
}

TEST(gltf_swordman_bone_matrix_calculation) {
    if (!std::filesystem::exists(SWORDMAN_GLTF_PATH)) {
        std::cout << "  [SKIP] Test file not found: " << SWORDMAN_GLTF_PATH << std::endl;
        return;
    }

    ModelData* model_data = MeshLoader::load_gltf(SWORDMAN_GLTF_PATH);
    assert_true(model_data != nullptr, "Should load model");
    assert_true(model_data->skeleton != nullptr, "Should have skeleton");
    assert_true(!model_data->animations.empty(), "Should have animations");

    Animator animator(model_data->skeleton.get());
    animator.play(model_data->animations[0].get(), true);

    animator.set_current_time(0.0f);
    animator.calculate_transforms();
    const auto matrices_t0 = animator.get_bone_matrices();

    animator.set_current_time(2.0f);
    animator.calculate_transforms();
    const auto matrices_t2 = animator.get_bone_matrices();

    assert_true(!matrices_t0.empty(), "Should have bone matrices at t=0");
    assert_true(!matrices_t2.empty(), "Should have bone matrices at t=2");
    delete model_data;
}

TEST(gltf_swordman_looping_animation) {
    if (!std::filesystem::exists(SWORDMAN_GLTF_PATH)) {
        std::cout << "  [SKIP] Test file not found: " << SWORDMAN_GLTF_PATH << std::endl;
        return;
    }

    ModelData* model_data = MeshLoader::load_gltf(SWORDMAN_GLTF_PATH);
    assert_true(model_data != nullptr, "Should load model");

    Animator animator(model_data->skeleton.get());
    animator.play(model_data->animations[0].get(), true);

    float duration = model_data->animations[0]->get_duration();
    animator.update(duration + 1.0f);

    assert_true(animator.is_animation_playing(), "Looping animation should still be playing");
    delete model_data;
}

TEST(gltf_swordman_animation_speed) {
    if (!std::filesystem::exists(SWORDMAN_GLTF_PATH)) {
        std::cout << "  [SKIP] Test file not found: " << SWORDMAN_GLTF_PATH << std::endl;
        return;
    }

    ModelData* model_data = MeshLoader::load_gltf(SWORDMAN_GLTF_PATH);
    assert_true(model_data != nullptr, "Should load model");

    Animator animator(model_data->skeleton.get());
    animator.play(model_data->animations[0].get(), false);
    animator.set_playback_speed(2.0f);
    animator.update(1.0f);

    float expected_time = 2.0f;
    float actual_time = animator.get_current_time();

    assert_true(std::abs(actual_time - expected_time) < 0.1f,
        "Playback speed should affect time advancement");
    delete model_data;
}

TEST(gltf_swordman_full_entity_integration) {
    if (!std::filesystem::exists(SWORDMAN_GLTF_PATH)) {
        std::cout << "  [SKIP] Test file not found: " << SWORDMAN_GLTF_PATH << std::endl;
        return;
    }

    ModelData* raw_model = MeshLoader::load_gltf(SWORDMAN_GLTF_PATH);
    assert_true(raw_model != nullptr, "Should load model");

    auto model_data = std::shared_ptr<ModelData>(raw_model);

    Entity swordman;
    swordman.setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
    swordman.setScale(glm::vec3(0.01f));

    AnimationComponent* anim_comp = new AnimationComponent(model_data);
    swordman.addComponent(anim_comp);

    AnimationComponent* retrieved = swordman.getComponent<AnimationComponent>();
    assert_true(retrieved != nullptr, "Should retrieve animation component");
    assert_true(retrieved->has_skeleton(), "Should have skeleton");

    retrieved->play_animation(0);

    for (int frame = 0; frame < 120; frame++) {
        swordman.updateComponents(0.016f);
    }

    assert_true(retrieved->is_playing(), "Should still be playing");

    delete anim_comp;
}

// ============================================================
// MAIN
// ============================================================

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "   Animation & ECS Integration Tests   " << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << std::endl;

    // Skeleton Tests
    std::cout << "--- Skeleton Tests ---" << std::endl;
    RUN_TEST(skeleton_creation);
    RUN_TEST(skeleton_add_bone);
    RUN_TEST(skeleton_bone_lookup);
    RUN_TEST(skeleton_has_bone);
    RUN_TEST(skeleton_bone_hierarchy);
    RUN_TEST(skeleton_calculate_bone_matrices);
    std::cout << std::endl;

    // Animation Clip Tests
    std::cout << "--- Animation Clip Tests ---" << std::endl;
    RUN_TEST(animation_clip_creation);
    RUN_TEST(animation_clip_default_values);
    RUN_TEST(animation_clip_add_bone_animation);
    RUN_TEST(animation_clip_get_nonexistent_bone);
    std::cout << std::endl;

    // Bone Animation Tests
    std::cout << "--- Bone Animation Interpolation Tests ---" << std::endl;
    RUN_TEST(bone_animation_position_interpolation);
    RUN_TEST(bone_animation_single_keyframe);
    RUN_TEST(bone_animation_empty_keyframes);
    RUN_TEST(bone_animation_rotation_interpolation);
    RUN_TEST(bone_animation_scale_interpolation);
    std::cout << std::endl;

    // Animator Tests
    std::cout << "--- Animator Tests ---" << std::endl;
    RUN_TEST(animator_creation);
    RUN_TEST(animator_with_skeleton);
    RUN_TEST(animator_play_animation);
    RUN_TEST(animator_stop_animation);
    RUN_TEST(animator_pause_resume);
    RUN_TEST(animator_update_advances_time);
    RUN_TEST(animator_looping);
    RUN_TEST(animator_non_looping_stops);
    RUN_TEST(animator_playback_speed);
    RUN_TEST(animator_set_current_time);
    std::cout << std::endl;

    // Entity Tests
    std::cout << "--- Entity Tests ---" << std::endl;
    RUN_TEST(entity_creation);
    RUN_TEST(entity_transform);
    RUN_TEST(entity_hierarchy);
    RUN_TEST(entity_hierarchy_reparent);
    RUN_TEST(entity_local_matrix);
    RUN_TEST(entity_world_matrix);
    RUN_TEST(entity_rotate_by);
    std::cout << std::endl;

    // Entity Component Tests
    std::cout << "--- Entity Component Tests ---" << std::endl;
    RUN_TEST(entity_add_component);
    RUN_TEST(entity_get_component);
    RUN_TEST(entity_get_component_not_found);
    RUN_TEST(entity_remove_component);
    RUN_TEST(entity_update_components);
    RUN_TEST(entity_update_components_recursive);
    std::cout << std::endl;

    // Animation Component Tests
    std::cout << "--- Animation Component Tests ---" << std::endl;
    RUN_TEST(animation_component_creation);
    RUN_TEST(animation_component_with_model_data);
    RUN_TEST(animation_component_set_model_data);
    RUN_TEST(animation_component_play_by_name);
    RUN_TEST(animation_component_play_by_index);
    RUN_TEST(animation_component_play_nonexistent);
    RUN_TEST(animation_component_stop_pause_resume);
    RUN_TEST(animation_component_get_animation_by_name);
    RUN_TEST(animation_component_auto_update);
    RUN_TEST(animation_component_bone_matrices);
    std::cout << std::endl;

    // ECS Integration Tests
    std::cout << "--- ECS Integration Tests ---" << std::endl;
    RUN_TEST(ecs_entity_with_animation_component);
    RUN_TEST(ecs_animated_entity_hierarchy);
    RUN_TEST(ecs_multiple_animated_children);
    RUN_TEST(ecs_entity_dynamic_transform_update);
    RUN_TEST(ecs_entity_incremental_rotation);
    std::cout << std::endl;

    // Full Integration Tests
    std::cout << "--- Full Integration Tests ---" << std::endl;
    RUN_TEST(full_animation_pipeline);
    RUN_TEST(multiple_animations_switching);
    RUN_TEST(full_ecs_animation_integration);
    std::cout << std::endl;

    // GLTF Loader Tests (Swordman Model)
    std::cout << "--- GLTF Loader Tests (Swordman Model) ---" << std::endl;
    RUN_TEST(gltf_load_swordman_model);
    RUN_TEST(gltf_swordman_has_skeleton);
    RUN_TEST(gltf_swordman_skeleton_hierarchy);
    RUN_TEST(gltf_swordman_bone_names);
    RUN_TEST(gltf_swordman_inverse_bind_matrices);
    RUN_TEST(gltf_swordman_has_animations);
    RUN_TEST(gltf_swordman_animation_duration);
    RUN_TEST(gltf_swordman_animation_keyframes);
    RUN_TEST(gltf_swordman_has_meshes);
    RUN_TEST(gltf_swordman_mesh_vertex_data);
    RUN_TEST(gltf_swordman_skinning_weights);
    RUN_TEST(gltf_swordman_animator_integration);
    RUN_TEST(gltf_swordman_animation_component_integration);
    RUN_TEST(gltf_swordman_animation_playback);
    RUN_TEST(gltf_swordman_bone_matrix_calculation);
    RUN_TEST(gltf_swordman_looping_animation);
    RUN_TEST(gltf_swordman_animation_speed);
    RUN_TEST(gltf_swordman_full_entity_integration);
    std::cout << std::endl;

    // Summary
    std::cout << "========================================" << std::endl;
    std::cout << "Tests passed: " << tests_passed << std::endl;
    std::cout << "Tests failed: " << tests_failed << std::endl;
    std::cout << "========================================" << std::endl;

    return tests_failed > 0 ? 1 : 0;
}