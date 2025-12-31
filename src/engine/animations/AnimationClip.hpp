#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <vector>
#include <string>

// Keyframe data for position
struct PositionKeyframe {
    float time;
    glm::vec3 position;
};

// Keyframe data for rotation
struct RotationKeyframe {
    float time;
    glm::quat rotation;
};

// Keyframe data for scale
struct ScaleKeyframe {
    float time;
    glm::vec3 scale;
};

// Animation channel for a single bone
struct BoneAnimation {
    int bone_id;
    std::vector<PositionKeyframe> position_keys;
    std::vector<RotationKeyframe> rotation_keys;
    std::vector<ScaleKeyframe> scale_keys;

    // Get interpolated transform at a given time
    glm::vec3 get_position(float time) const;
    glm::quat get_rotation(float time) const;
    glm::vec3 get_scale(float time) const;
};

// Animation clip containing all bone animations
class AnimationClip {
private:
    std::string name;
    float duration;
    float ticks_per_second;
    std::vector<BoneAnimation> bone_animations;

public:
    AnimationClip();
    AnimationClip(const std::string& name, float duration, float ticks_per_second = 25.0f);

    // Getters
    const std::string& get_name() const { return name; }
    float get_duration() const { return duration; }
    float get_ticks_per_second() const { return ticks_per_second; }
    const std::vector<BoneAnimation>& get_bone_animations() const { return bone_animations; }

    // Setters
    void set_name(const std::string& name) { this->name = name; }
    void set_duration(float duration) { this->duration = duration; }
    void set_ticks_per_second(float tps) { this->ticks_per_second = tps; }

    // Add bone animation channel
    void add_bone_animation(const BoneAnimation& bone_anim);

    // Get bone animation by index
    const BoneAnimation* get_bone_animation(int bone_id) const;
};