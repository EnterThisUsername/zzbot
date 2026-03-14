#include "SpeedControl.hpp"

SpeedControl* SpeedControl::get() {
    static SpeedControl s_instance;
    return &s_instance;
}

void SpeedControl::setSpeed(float multiplier) {
    // Clamp to a safe range.
    multiplier = std::clamp(multiplier, 0.001f, 100.0f);

    m_previousSpeed = m_speed;
    m_speed         = multiplier;
    applyToScheduler();

    log::info("[zzBot] Speed set to {:.3f}x", m_speed);
}

void SpeedControl::reset() {
    setSpeed(1.0f);
}

void SpeedControl::applyToScheduler() {
    // CCScheduler::setTimeScale affects every scheduled update,
    // giving us uniform slow/fast motion across physics and animation.
    if (auto* sched = CCDirector::get()->getScheduler())
        sched->setTimeScale(m_speed);
}
