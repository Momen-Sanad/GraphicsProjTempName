#include "AnimationComponent.hpp"
#include "Entity.hpp"
#include "../assets/SkinnedMaterial.hpp"
#include <iostream>


AnimationComponent::AnimationComponent()
    : model_data(nullptr)
    , auto_update(true)
{
}

AnimationComponent::AnimationComponent(std::shared_ptr<ModelData> data)
    : model_data(data)
    , auto_update(true)
{
    if (model_data && model_data->skeleton) {
        animator.set_skeleton(model_data->skeleton.get());
    }
    build_animation_map();
}

void AnimationComponent::set_model_data(std::shared_ptr<ModelData> data) {
    model_data = data;
    if (model_data && model_data->skeleton) {
        animator.set_skeleton(model_data->skeleton.get());
    }
    build_animation_map();
}

void AnimationComponent::build_animation_map() {
    animation_name_to_index.clear();

    if (!model_data) return;

    for (size_t i = 0; i < model_data->animations.size(); ++i) {
        const auto& clip = model_data->animations[i];
        if (clip) {
            animation_name_to_index[clip->get_name()] = static_cast<int>(i);
        }
    }
}


void AnimationComponent::update(Entity& entity, float deltaTime) {
    if (!auto_update || !model_data || !model_data->skeleton) {
        return;
    }

    // Update animator (advances time, computes bone matrices)
    animator.update(deltaTime);

    // Update material bone matrices if available
    update_material_bone_matrices(&entity);

    // If entity has a SkinnedMaterial, update its bone matrices
    SkinnedMaterial* skinned_mat = entity.getSkinnedMaterial();
        if (skinned_mat) {
            const auto& bone_matrices = animator.get_bone_matrices();
            skinned_mat->set_bone_matrices(bone_matrices);
            skinned_mat->set_animated(true);
        }
}

void AnimationComponent::render(Entity& entity) {
    // Rendering is handled by Entity's draw() method
    // The bone matrices have already been updated in update()
    // Nothing additional needed here
}

void AnimationComponent::update_material_bone_matrices(Entity* ent) {
    SkinnedMaterial* skinned_material = ent->getSkinnedMaterial();

    if (skinned_material) {
        const auto& bone_matrices = animator.get_bone_matrices();
        skinned_material->set_bone_matrices(bone_matrices);
        skinned_material->set_animated(true);
    }
}

// ============================================================
// ANIMATION CONTROL
// ============================================================

bool AnimationComponent::play_animation(const std::string& name, bool loop) {
    const AnimationClip* clip = get_animation(name);
    if (clip) {
        animator.play(clip, loop);
        return true;
    }

    std::cerr << "Animation '" << name << "' not found!" << std::endl;
    return false;
}

bool AnimationComponent::play_animation(int index, bool loop) {
    const AnimationClip* clip = get_animation(index);
    if (clip) {
        animator.play(clip, loop);
        return true;
    }

    std::cerr << "Animation index " << index << " out of range!" << std::endl;
    return false;
}

void AnimationComponent::stop_animation() {
    animator.stop();
}

void AnimationComponent::pause_animation() {
    animator.pause();
}

void AnimationComponent::resume_animation() {
    animator.resume();
}

void AnimationComponent::set_animation_speed(float speed) {
    animator.set_playback_speed(speed);
}

void AnimationComponent::set_animation_time(float time) {
    animator.set_current_time(time);
}

const Skeleton* AnimationComponent::get_skeleton() const {
    if (model_data) {
        return model_data->skeleton.get();
    }
    return nullptr;
}

Skeleton* AnimationComponent::get_skeleton() {
    if (model_data) {
        return model_data->skeleton.get();
    }
    return nullptr;
}

bool AnimationComponent::has_skeleton() const {
    return model_data && model_data->skeleton != nullptr;
}

bool AnimationComponent::has_animations() const {
    return model_data && !model_data->animations.empty();
}

int AnimationComponent::get_animation_count() const {
    if (model_data) {
        return static_cast<int>(model_data->animations.size());
    }
    return 0;
}

const AnimationClip* AnimationComponent::get_animation(const std::string& name) const {
    auto it = animation_name_to_index.find(name);
    if (it != animation_name_to_index.end()) {
        return get_animation(it->second);
    }
    return nullptr;
}

const AnimationClip* AnimationComponent::get_animation(int index) const {
    if (model_data && index >= 0 && index < static_cast<int>(model_data->animations.size())) {
        return model_data->animations[index].get();
    }
    return nullptr;
}