#pragma once

#include <array>
#include <functional>
#include <vector>

enum class FramePhase {
    Input = 0,
    FixedUpdate,
    Update,
    Render,
    Count
};

class FrameScheduler {
public:
    using SystemFn = std::function<void(float)>;

    explicit FrameScheduler(float fixedDeltaSeconds = 1.0f / 60.0f);

    void setFixedDelta(float fixedDeltaSeconds);
    float fixedDelta() const { return m_fixedDeltaSeconds; }

    void addSystem(FramePhase phase, SystemFn fn);
    void clear();

    void tick(float deltaSeconds);

private:
    float m_fixedDeltaSeconds = 1.0f / 60.0f;
    float m_accumulator = 0.0f;
    std::array<std::vector<SystemFn>, static_cast<size_t>(FramePhase::Count)> m_phases;
};
