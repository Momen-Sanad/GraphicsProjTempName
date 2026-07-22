#include "AnimationClip.hpp"
#include <algorithm>

// Helper function to find keyframe index
template<typename T>
static int find_keyframe_index(const std::vector<T>& keyframes, float time) {
    for (size_t i = 0; i < keyframes.size() - 1; ++i) {
        if (time < keyframes[i + 1].time) {
            return static_cast<int>(i);
        }
    }
    return static_cast<int>(keyframes.size()) - 2;
}

// Get interpolated position at given time
glm::vec3 BoneAnimation::get_position(float time) const {
    if (position_keys.empty()) {
        return glm::vec3(0.0f);
    }

    if (position_keys.size() == 1) {
        return position_keys[0].position;
    }

    int index = find_keyframe_index(position_keys, time);
    int next_index = index + 1;

    const auto& key1 = position_keys[index];
    const auto& key2 = position_keys[next_index];

    float delta_time = key2.time - key1.time;
    float factor = (time - key1.time) / delta_time;

    return glm::mix(key1.position, key2.position, factor);
}

// Get interpolated rotation at given time
glm::quat BoneAnimation::get_rotation(float time) const {
    if (rotation_keys.empty()) {
        return glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    }

    if (rotation_keys.size() == 1) {
        return rotation_keys[0].rotation;
    }

    int index = find_keyframe_index(rotation_keys, time);
    int next_index = index + 1;

    const auto& key1 = rotation_keys[index];
    const auto& key2 = rotation_keys[next_index];

    float delta_time = key2.time - key1.time;
    float factor = (time - key1.time) / delta_time;

    return glm::slerp(key1.rotation, key2.rotation, factor);
}

// Get interpolated scale at given time
glm::vec3 BoneAnimation::get_scale(float time) const {
    if (scale_keys.empty()) {
        return glm::vec3(1.0f);
    }

    if (scale_keys.size() == 1) {
        return scale_keys[0].scale;
    }

    int index = find_keyframe_index(scale_keys, time);
    int next_index = index + 1;

    const auto& key1 = scale_keys[index];
    const auto& key2 = scale_keys[next_index];

    float delta_time = key2.time - key1.time;
    float factor = (time - key1.time) / delta_time;

    return glm::mix(key1.scale, key2.scale, factor);
}

// AnimationClip implementation
AnimationClip::AnimationClip()
    : name("")
    , duration(0.0f)
    , ticks_per_second(25.0f)
{
}

AnimationClip::AnimationClip(const std::string& name, float duration, float ticks_per_second)
    : name(name)
    , duration(duration)
    , ticks_per_second(ticks_per_second)
{
}

void AnimationClip::add_bone_animation(const BoneAnimation& bone_anim) {
    bone_animations.push_back(bone_anim);
}

const BoneAnimation* AnimationClip::get_bone_animation(int bone_id) const {
    for (const auto& anim : bone_animations) {
        if (anim.bone_id == bone_id) {
            return &anim;
        }
    }
    return nullptr;
}