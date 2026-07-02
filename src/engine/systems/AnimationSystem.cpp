#include "AnimationSystem.hpp"

#include "../assets/AssetManager.hpp"
#include "../assets/SkinnedMaterial.hpp"
#include "../ecs/EcsComponents.hpp"
#include "../ecs/Registry.hpp"

namespace {
const SkinAsset* resolveSkin(engine::ecs::AnimatorData& animation, engine::ecs::SkinnedRenderable& skinned)
{
    if (!animation.model) {
        animation.model = skinned.model;
    }
    if (animation.skinIndex < 0) {
        animation.skinIndex = skinned.skinIndex;
    }

    if (!animation.model) {
        return nullptr;
    }

    const int skinIndex = animation.skinIndex >= 0 ? animation.skinIndex : 0;
    if (skinIndex < 0 || static_cast<size_t>(skinIndex) >= animation.model->skins.size()) {
        return nullptr;
    }

    return &animation.model->skins[static_cast<size_t>(skinIndex)];
}
}

void AnimationSystem::update(engine::ecs::Registry& registry, float deltaTime)
{
    registry.each<engine::ecs::AnimatorData, engine::ecs::SkinnedRenderable>(
        [deltaTime](
            engine::ecs::EntityId,
            engine::ecs::AnimatorData& animation,
            engine::ecs::SkinnedRenderable& skinned) {
            const SkinAsset* skin = resolveSkin(animation, skinned);
            if (!skin || !skin->skeleton) {
                return;
            }

            if (!animation.initialized) {
                animation.animator.set_skeleton(skin->skeleton.get());
                animation.initialized = true;
            }

            const bool hasClip =
                animation.currentAnimation >= 0 &&
                static_cast<size_t>(animation.currentAnimation) < animation.model->animations.size() &&
                animation.model->animations[static_cast<size_t>(animation.currentAnimation)];

            if (!hasClip || !animation.playing) {
                return;
            }

            const AnimationClip* clip = animation.model->animations[static_cast<size_t>(animation.currentAnimation)].get();
            animation.animator.set_looping(animation.loop);
            animation.animator.set_playback_speed(animation.speed);

            if (animation.animator.get_current_clip() != clip) {
                animation.animator.play(clip, animation.loop);
            } else if (!animation.animator.is_animation_playing()) {
                if (!animation.loop && animation.animator.get_current_time() >= clip->get_duration()) {
                    animation.playing = false;
                    return;
                }
                animation.animator.resume();
            }

            animation.animator.update(deltaTime);
            if (!animation.loop && !animation.animator.is_animation_playing()) {
                animation.playing = false;
            }

            if (skinned.material) {
                skinned.material->set_bone_matrices(animation.animator.get_bone_matrices());
                skinned.material->set_animated(true);
            }
        });
}

void AnimationSystem::play(
    engine::ecs::Registry& registry,
    engine::ecs::EntityId entity,
    int animationIndex,
    bool loop,
    float speed)
{
    auto& animation = registry.ensure<engine::ecs::AnimatorData>(entity);
    animation.currentAnimation = animationIndex;
    animation.loop = loop;
    animation.speed = speed;
    animation.playing = true;
}
