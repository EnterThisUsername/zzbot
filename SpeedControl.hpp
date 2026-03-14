#pragma once
#include <Geode/Geode.hpp>

using namespace geode::prelude;

// ─────────────────────────────────────────────────────────────────────────────
// SpeedControl — singleton
//
// Scales the Cocos2d CCScheduler time-scale so that all game updates
// (physics, animations, input polling) advance proportionally.
//
// 1.0  = normal (100%)
// 0.5  = half speed (50%)
// 0.01 = 1% speed — useful for precise slow-motion recording
// 2.0  = double speed, etc.
// ─────────────────────────────────────────────────────────────────────────────
class SpeedControl {
public:
    static SpeedControl* get();

    /// Apply a new speed multiplier immediately.
    void setSpeed(float multiplier);

    float speed() const { return m_speed; }

    /// Restore normal speed (1.0).
    void reset();

private:
    SpeedControl()          = default;
    float m_speed           = 1.0f;
    float m_previousSpeed   = 1.0f;

    void applyToScheduler();
};
