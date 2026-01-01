#include "FrameScheduler.hpp"

#include <algorithm>

FrameScheduler::FrameScheduler() {
    reset();
}

void FrameScheduler::reset() {
    last_time_ = clock::now();
    has_time_ = true;
    delta_time_ = 0.0f;
    accumulator_ = 0.0f;
}

void FrameScheduler::tick() {
    clock::time_point now = clock::now();
    if (!has_time_) {
        last_time_ = now;
        has_time_ = true;
        delta_time_ = 0.0f;
        return;
    }

    std::chrono::duration<float> elapsed = now - last_time_;
    last_time_ = now;

    delta_time_ = std::min(elapsed.count(), max_frame_time_);
    if (delta_time_ < 0.0f) {
        delta_time_ = 0.0f;
    }

    if (fixed_enabled_) {
        accumulator_ += delta_time_;
    }
}

void FrameScheduler::set_fixed_delta(float dt) {
    fixed_delta_ = std::max(dt, 0.0001f);
}

void FrameScheduler::set_max_frame_time(float dt) {
    max_frame_time_ = std::max(dt, 0.0f);
}

bool FrameScheduler::consume_fixed_step() {
    if (!fixed_enabled_) {
        return false;
    }
    if (accumulator_ >= fixed_delta_) {
        accumulator_ -= fixed_delta_;
        return true;
    }
    return false;
}

int FrameScheduler::pending_fixed_steps() const {
    if (!fixed_enabled_) {
        return 0;
    }
    return static_cast<int>(accumulator_ / fixed_delta_);
}

float FrameScheduler::interpolation_alpha() const {
    if (!fixed_enabled_ || fixed_delta_ <= 0.0f) {
        return 0.0f;
    }
    return std::clamp(accumulator_ / fixed_delta_, 0.0f, 1.0f);
}
