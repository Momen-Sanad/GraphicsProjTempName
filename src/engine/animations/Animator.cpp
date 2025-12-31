#include "Animator.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

Animator::Animator()
    : skeleton(nullptr)
    , current_clip(nullptr)
    , current_time(0.0f)
    , playback_speed(1.0f)
    , is_playing(false)
    , is_looping(true)
{
}

Animator::Animator(const Skeleton* skeleton)
    : skeleton(skeleton)
    , current_clip(nullptr)
    , current_time(0.0f)
    , playback_speed(1.0f)
    , is_playing(false)
    , is_looping(true)
{
    if (skeleton) {
        local_transforms.resize(skeleton->get_bone_count(), glm::mat4(1.0f));
        bone_matrices.resize(skeleton->get_bone_count(), glm::mat4(1.0f));
    }
}

void Animator::set_skeleton(const Skeleton* skeleton) {
    this->skeleton = skeleton;
    if (skeleton) {
        local_transforms.resize(skeleton->get_bone_count(), glm::mat4(1.0f));
        bone_matrices.resize(skeleton->get_bone_count(), glm::mat4(1.0f));
    }
}

void Animator::play(const AnimationClip* clip, bool loop) {
    if (!clip) return;

    current_clip = clip;
    current_time = 0.0f;
    is_playing = true;
    is_looping = loop;

    calculate_transforms();
}

void Animator::stop() {
    is_playing = false;
    current_time = 0.0f;
    current_clip = nullptr;
}

void Animator::pause() {
    is_playing = false;
}

void Animator::resume() {
    if (current_clip) {
        is_playing = true;
    }
}

void Animator::set_current_time(float time) {
    current_time = time;
    if (current_clip && current_time > current_clip->get_duration()) {
        if (is_looping) {
            current_time = fmod(current_time, current_clip->get_duration());
        }
        else {
            current_time = current_clip->get_duration();
        }
    }
    calculate_transforms();
}

void Animator::update(float delta_time) {
    if (!is_playing || !current_clip || !skeleton) {
        return;
    }

    // Update time - don't multiply by ticks_per_second for GLTF 
    // (GLTF times are already in seconds)
    current_time += delta_time * playback_speed;

    // Handle looping or stopping at end
    if (current_time > current_clip->get_duration()) {
        if (is_looping) {
            current_time = fmod(current_time, current_clip->get_duration());
        }
        else {
            current_time = current_clip->get_duration();
            is_playing = false;
        }
    }

    calculate_transforms();
}

void Animator::calculate_transforms() {
    if (!skeleton || !current_clip) {
        return;
    }

    // Update local transforms for each bone based on animation
    for (int i = 0; i < skeleton->get_bone_count(); ++i) {
        const Bone& bone = skeleton->get_bone(i);
        const BoneAnimation* bone_anim = current_clip->get_bone_animation(i);

        glm::vec3 position(0.0f);
        glm::quat rotation(1.0f, 0.0f, 0.0f, 0.0f);
        glm::vec3 scale(1.0f);

        if (bone_anim) {
            position = bone_anim->get_position(current_time);
            rotation = bone_anim->get_rotation(current_time);
            scale = bone_anim->get_scale(current_time);
        }
        else {
            // Use bind pose if no animation data
            local_transforms[i] = bone.local_transform;
            continue;
        }

        // Build transform matrix from TRS
        glm::mat4 translation = glm::translate(glm::mat4(1.0f), position);
        glm::mat4 rotation_mat = glm::toMat4(rotation);
        glm::mat4 scale_mat = glm::scale(glm::mat4(1.0f), scale);

        local_transforms[i] = translation * rotation_mat * scale_mat;
    }

    // Calculate final bone matrices
    skeleton->calculate_bone_matrices(local_transforms, bone_matrices);
}