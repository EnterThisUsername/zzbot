#pragma once
#include <Geode/Geode.hpp>

using namespace geode::prelude;

// ─────────────────────────────────────────────────────────────────────────────
// FrameStepper — singleton
//
// Lets the user advance the simulation one physics tick at a time.
//
//  F key  → step forward one tick
//  G key  → exit stepping mode and resume at 50% speed
//
// Implementation:
//  • Stepping freezes the scheduler (timescale → 0).
//  • "Step forward" briefly restores timescale for exactly one physics tick
//    then re-freezes.
// ─────────────────────────────────────────────────────────────────────────────
class FrameStepper {
public:
    static FrameStepper* get();

    bool isActive()    const { return m_active; }
    int  currentTick() const { return m_tick;   }

    /// Enter frame-step mode. Freezes normal simulation.
    void enter();

    /// Exit frame-step mode and resume at 50% speed.
    void exit();

    /// Advance exactly one physics tick.
    void stepForward();

    /// Called by the physics hook each tick.
    /// Returns true while normal update should run.
    bool onPhysicsTick(int tick);

private:
    FrameStepper() = default;

    bool m_active = false;
    int  m_tick   = 0;
    bool m_doStep = false;
};
