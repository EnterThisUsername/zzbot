#pragma once
#include <Geode/Geode.hpp>

using namespace geode::prelude;

// ─────────────────────────────────────────────────────────────────────────────
// TPSModifier — singleton
//
// Geometry Dash runs its physics simulation at exactly 240 ticks per second.
// Each tick advances the physics by a fixed step of (1 / 240) seconds.
//
// This class stores a custom tick rate and exposes the derived physics step
// so that the GJBaseGameLayer hook can substitute the custom value into the
// physics accumulator logic.
//
// Increasing TPS (> 240) makes physics update more frequently — useful for
// sub-frame precision.  Decreasing TPS (< 240) effectively samples physics
// less often.  The renderer frame rate is independent.
// ─────────────────────────────────────────────────────────────────────────────
class TPSModifier {
public:
    static TPSModifier* get();

    void  setTPS(float tps);
    float tps()       const { return m_tps; }
    float physicsStep() const { return m_step; }   ///< 1 / tps

    bool isModified() const { return m_tps != k_defaultTPS; }

    static constexpr float k_defaultTPS = 240.0f;

private:
    TPSModifier() = default;
    float m_tps   = k_defaultTPS;
    float m_step  = 1.0f / k_defaultTPS;
};
