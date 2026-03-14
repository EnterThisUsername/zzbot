#pragma once
#include <Geode/Geode.hpp>
#include <vector>

using namespace geode::prelude;

// ─────────────────────────────────────────────────────────────────────────────
// FrameStepper — singleton
//
// Lets the user advance or retreat the simulation one physics tick at a time.
//
//  X key  → step forward one tick
//  C key  → step backward one tick (restarts + re-replays to tick-1)
//  V key  → exit stepping mode, resume at 50% speed
//
// Implementation strategy
//  • Stepping freezes the scheduler (timescale → 0).
//  • "Step forward" briefly restores timescale for exactly one physics tick,
//    then re-freezes.  The ReplaySystem records/replays handle determinism.
//  • "Step backward" stops the current run, calls PlayLayer::resetLevel(),
//    starts playback of the stored macro up to (currentTick - 1), then
//    re-enters step mode.
// ─────────────────────────────────────────────────────────────────────────────
class FrameStepper {
public:
    static FrameStepper* get();

    bool  isActive()      const { return m_active; }
    int   currentTick()   const { return m_tick; }

    /// Enter frame-step mode.  Freezes normal simulation.
    void enter();

    /// Exit frame-step mode and resume at 50% speed.
    void exit();

    /// Advance exactly one physics tick.
    void stepForward();

    /// Retreat one tick by restarting and replaying to (tick − 1).
    void stepBack();

    /// Called by the physics hook to count ticks and manage the
    /// single-step advance.  Returns true while normal update should run.
    bool onPhysicsTick(int tick);

private:
    FrameStepper() = default;

    bool m_active       = false;
    int  m_tick         = 0;       ///< last completed tick in step mode
    bool m_doStep       = false;   ///< flag: advance one tick next update
    bool m_stepInProgress = false; ///< guard: currently inside a step
};
