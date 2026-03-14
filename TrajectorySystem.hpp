#pragma once
#include <Geode/Geode.hpp>
#include <vector>

using namespace geode::prelude;

// ─────────────────────────────────────────────────────────────────────────────
// TrajectorySystem — singleton
//
// Runs a lightweight forward physics simulation from the player's current
// state and renders the predicted path as a series of dots/line on screen.
//
// The simulation is deliberately kept simple (ignores complex object interactions
// such as speed portals mid-trajectory) but accounts for:
//  • Gravity and gravity direction (normal / upside-down / flipped)
//  • Terminal velocity
//  • Game-mode-specific physics (cube, ship, ball, UFO, wave, robot, spider)
//  • Active inputs during the lookahead window (held buttons are forwarded)
//
// Implementation:
//  A VisualizerNode is added as a child of PlayLayer each run.  Every render
//  frame it re-samples the player state, runs N ticks of forward sim, then
//  redraws using CCDrawNode primitives.
// ─────────────────────────────────────────────────────────────────────────────

struct PhysicsSnapshot {
    CCPoint position  = CCPointZero;
    float   velY      = 0.0f;
    float   velX      = 0.0f;
    bool    upsideDown = false;
    bool    isOnGround = false;
    int     gameMode  = 0;   ///< IconType
    float   gravMultiplier = 1.0f;
    bool    holdingJump    = false;
};

class TrajectorySystem {
public:
    static TrajectorySystem* get();

    bool isEnabled() const { return m_enabled; }
    void setEnabled(bool v) { m_enabled = v; }

    int  lookaheadTicks() const { return m_lookaheadTicks; }
    void setLookaheadTicks(int n) { m_lookaheadTicks = std::clamp(n, 10, 1920); }

    /// Sample the player state and run the forward simulation.
    /// Returns the list of predicted world positions.
    std::vector<CCPoint> simulate(PlayerObject* player, float physicsStep);

private:
    TrajectorySystem() = default;

    bool m_enabled        = false;
    int  m_lookaheadTicks = 240;

    /// Advance one physics tick for the given snapshot.
    void stepPhysics(PhysicsSnapshot& snap, float dt) const;

    /// Clamp velocity to GD-style terminal velocities.
    void clampVelocity(PhysicsSnapshot& snap) const;

    PhysicsSnapshot capturePlayer(PlayerObject* player) const;
};
