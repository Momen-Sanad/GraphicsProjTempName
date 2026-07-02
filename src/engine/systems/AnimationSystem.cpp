#include "AnimationSystem.hpp"

#include "../assets/ModelData.hpp"
#include "../assets/SkinnedMaterial.hpp"
#include "../ecs/EcsComponents.hpp"
#include "../ecs/Registry.hpp"

void AnimationSystem::update(engine::ecs::Registry& registry, float deltaTime)
{
    registry.each<engine::ecs::AnimatorData, engine::ecs::SkinnedRenderable>(
        [deltaTime](
            engine::ecs::EntityId,
            engine::ecs::AnimatorData& animation,
            engine::ecs::SkinnedRenderable& skinned) {
            if (!animation.modelData) {
                animation.modelData = skinned.modelData;
            }
            if (!animation.modelData || !animation.modelData->skeleton) {
                return;
            }

            if (!animation.initialized) {
                animation.animator.set_skeleton(animation.modelData->skeleton.get());
                animation.initialized = true;
            }

            const bool hasClip =
                animation.currentAnimation >= 0 &&
                static_cast<size_t>(animation.currentAnimation) < animation.modelData->animations.size() &&
                animation.modelData->animations[static_cast<size_t>(animation.currentAnimation)];

            if (hasClip) {
                const AnimationClip* clip = animation.modelData->animations[static_cast<size_t>(animation.currentAnimation)].get();
                if (animation.animator.get_current_clip() != clip || !animation.animator.is_animation_playing()) {
                    animation.animator.play(clip, animation.loop);
                }
                animation.animator.set_looping(animation.loop);
                animation.animator.set_playback_speed(animation.speed);
                animation.playing = true;
            }

            if (!animation.playing) {
                return;
            }

            animation.animator.update(deltaTime);

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
