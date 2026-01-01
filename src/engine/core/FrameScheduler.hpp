#pragma once

#include <chrono>

class FrameScheduler {
public:
    FrameScheduler();

    void reset();
    void tick();

    float delta_time() const { return delta_time_; }
    float fixed_delta() const { return fixed_delta_; }
    float max_frame_time() const { return max_frame_time_; }

    void set_fixed_step(bool enabled) { fixed_enabled_ = enabled; }
    bool fixed_step_enabled() const { return fixed_enabled_; }

    void set_fixed_delta(float dt);
    void set_max_frame_time(float dt);

    bool consume_fixed_step();
    int pending_fixed_steps() const;
    float interpolation_alpha() const;

private:
    using clock = std::chrono::steady_clock;

    clock::time_point last_time_{};
    bool has_time_ = false;

    float delta_time_ = 0.0f;
    float fixed_delta_ = 1.0f / 60.0f;
    float max_frame_time_ = 0.25f;
    bool fixed_enabled_ = false;
    float accumulator_ = 0.0f;
};
