#pragma once
#include "AnimationClip.hpp"
#include "Skeleton.hpp"
#include <vector>
#include <memory>

// Animator class handling animation playback
class Animator {
private:
    const Skeleton* skeleton;
    const AnimationClip* current_clip;
    float current_time;
    float playback_speed;
    bool is_playing;
    bool is_looping;

    // Local transforms for each bone (updated each frame)
    std::vector<glm::mat4> local_transforms;

    // Final bone matrices for rendering (bone space -> model space)
    std::vector<glm::mat4> bone_matrices;

public:
    Animator();
    explicit Animator(const Skeleton* skeleton);

    // Set the skeleton
    void set_skeleton(const Skeleton* skeleton);

    // Play animation
    void play(const AnimationClip* clip, bool loop = true);

    // Stop animation
    void stop();

    // Pause/Resume
    void pause();
    void resume();

    // Update animation (call every frame)
    void update(float delta_time);

    // Getters
    bool is_animation_playing() const { return is_playing; }
    float get_current_time() const { return current_time; }
    const AnimationClip* get_current_clip() const { return current_clip; }
    const std::vector<glm::mat4>& get_bone_matrices() const { return bone_matrices; }

    // Setters
    void set_playback_speed(float speed) { playback_speed = speed; }
    void set_current_time(float time);
    void set_looping(bool loop) { is_looping = loop; }

    // Calculate transforms for current animation time
    void calculate_transforms();
};