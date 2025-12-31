#include "FrameScheduler.hpp"

FrameScheduler::FrameScheduler(float fixedDeltaSeconds)
    : m_fixedDeltaSeconds(fixedDeltaSeconds) {}

void FrameScheduler::setFixedDelta(float fixedDeltaSeconds) {
    m_fixedDeltaSeconds = fixedDeltaSeconds;
}

void FrameScheduler::addSystem(FramePhase phase, SystemFn fn) {
    m_phases[static_cast<size_t>(phase)].push_back(std::move(fn));
}

void FrameScheduler::clear() {
    for (auto& phase : m_phases) {
        phase.clear();
    }
    m_accumulator = 0.0f;
}

void FrameScheduler::tick(float deltaSeconds) {
    for (auto& system : m_phases[static_cast<size_t>(FramePhase::Input)]) {
        system(deltaSeconds);
    }

    m_accumulator += deltaSeconds;
    while (m_accumulator >= m_fixedDeltaSeconds) {
        for (auto& system : m_phases[static_cast<size_t>(FramePhase::FixedUpdate)]) {
            system(m_fixedDeltaSeconds);
        }
        m_accumulator -= m_fixedDeltaSeconds;
    }

    for (auto& system : m_phases[static_cast<size_t>(FramePhase::Update)]) {
        system(deltaSeconds);
    }

    for (auto& system : m_phases[static_cast<size_t>(FramePhase::Render)]) {
        system(deltaSeconds);
    }
}
