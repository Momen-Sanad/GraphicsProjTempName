#pragma once
#include "Component.hpp"
#include "../animations/Animator.hpp"
#include "../animations/Skeleton.hpp"
#include "../animations/AnimationClip.hpp"
#include "../gl/SkinnedMesh.hpp"
#include "../gl/Texture.hpp"
#include <memory>
#include <unordered_map>
#include <string>
#include <vector>   

// Forward declaration
class Entity;
class SkinnedMeshRenderer;
class SkinnedMaterial;

struct ModelData
{
    std::shared_ptr<Skeleton> skeleton;
    std::vector<std::shared_ptr<AnimationClip>> animations;
    std::vector<SkinnedMesh> meshes;                       // Loaded mesh data
    std::vector<std::shared_ptr<Texture>> textures;        // ADD THIS LINE

    ModelData() = default;

    // Allow moving
    ModelData(ModelData&&) = default;
    ModelData& operator=(ModelData&&) = default;
};

// AnimationComponent handles skeletal animation for entities
class AnimationComponent : public Component 
{
private:
    std::shared_ptr<ModelData> model_data;
    Animator animator;
    std::unordered_map<std::string, int> animation_name_to_index;

    bool auto_update;  // Automatically update animator each frame

public:
    AnimationComponent();
    AnimationComponent(std::shared_ptr<ModelData> model_data);
    //~AnimationComponent() override = default;

    // Initialize with model data
    void set_model_data(std::shared_ptr<ModelData> model_data);

    // Component interface
    // Forwarding update from base class
    void update(Entity& entity, float deltaTime) override;
    void render(Entity& entity) override;

    // Animation control
    bool play_animation(const std::string& name, bool loop = true);
    bool play_animation(int index, bool loop = true);
    void stop_animation();
    void pause_animation();
    void resume_animation();
    void set_animation_speed(float speed);
    void set_animation_time(float time);

    // Getters
    const Skeleton* get_skeleton() const;
    Skeleton* get_skeleton();
    Animator& get_animator() { return animator; }
    const Animator& get_animator() const { return animator; }

    const AnimationClip* get_animation(const std::string& name) const;
    const AnimationClip* get_animation(int index) const;
    int get_animation_count() const;

    bool has_skeleton() const;
    bool has_animations() const;
    bool is_playing() const { return animator.is_animation_playing(); }

    const std::vector<glm::mat4>& get_bone_matrices() const {
        return animator.get_bone_matrices();
    }

    std::shared_ptr<ModelData> get_model_data() const { return model_data; }

    // Settings
    void set_auto_update(bool enabled) { auto_update = enabled; }
    bool get_auto_update() const { return auto_update; }

private:
    void build_animation_map();
    void update_material_bone_matrices(Entity* ent);
};