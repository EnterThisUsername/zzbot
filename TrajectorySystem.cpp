#include "TrajectorySystem.hpp"
#include <algorithm>

TrajectorySystem* TrajectorySystem::get() {
    static TrajectorySystem s_instance;
    return &s_instance;
}

// ─────────────────────────────────────────────────────────────────────────────
// Constants — tuned from GD reverse-engineering / community docs
// ─────────────────────────────────────────────────────────────────────────────

namespace GDPhysics {
    // Cube
    static constexpr float CUBE_GRAVITY      = 0.958199f;  // per tick
    static constexpr float CUBE_JUMP_VEL     = 11.180340f;
    static constexpr float CUBE_MAX_FALL     = 12.0f;

    // Ship
    static constexpr float SHIP_GRAVITY      = 0.400000f;
    static constexpr float SHIP_THRUST       = 0.500000f;
    static constexpr float SHIP_MAX_UP       =  7.5f;
    static constexpr float SHIP_MAX_DOWN     = -7.5f;

    // Ball
    static constexpr float BALL_GRAVITY      = 0.958199f;
    static constexpr float BALL_MAX_FALL     = 12.0f;

    // UFO
    static constexpr float UFO_GRAVITY       = 0.350000f;
    static constexpr float UFO_BOOST         = 2.600000f;
    static constexpr float UFO_MAX_UP        = 11.0f;
    static constexpr float UFO_MAX_DOWN      = -11.0f;

    // Wave: moves diagonally, no gravity.
    static constexpr float WAVE_SPEED_Y      = 8.0f;

    // Horizontal speed per tick at 1× speed.
    static constexpr float HORIZONTAL_SPEED  = 5.770000f;
}

// ─────────────────────────────────────────────────────────────────────────────

PhysicsSnapshot TrajectorySystem::capturePlayer(PlayerObject* player) const {
    PhysicsSnapshot snap;
    snap.position    = player->getPosition();
    snap.velY        = player->m_yVelocity;
    snap.velX        = player->m_xVelocity;
    snap.upsideDown  = player->m_isUpsideDown;
    snap.isOnGround  = player->m_isOnGround;
    snap.gameMode    = static_cast<int>(player->m_gamemode);
    snap.gravMultiplier = player->m_gravityMod;
    snap.holdingJump    = player->m_jumpBuffered;
    return snap;
}

std::vector<CCPoint> TrajectorySystem::simulate(PlayerObject* player, float physicsStep) {
    if (!m_enabled || !player) return {};

    PhysicsSnapshot snap = capturePlayer(player);
    std::vector<CCPoint> path;
    path.reserve(static_cast<size_t>(m_lookaheadTicks));

    for (int i = 0; i < m_lookaheadTicks; ++i) {
        stepPhysics(snap, physicsStep);
        path.push_back(snap.position);
    }

    return path;
}

// ─────────────────────────────────────────────────────────────────────────────
// Per-tick physics step
// ─────────────────────────────────────────────────────────────────────────────

void TrajectorySystem::stepPhysics(PhysicsSnapshot& snap, float /*dt*/) const {
    const float gravSign = snap.upsideDown ? -1.0f : 1.0f;
    const float g        = snap.gravMultiplier;

    switch (snap.gameMode) {

    // ── Cube ──────────────────────────────────────────────────────────────────
    case 0: {
        snap.velY -= GDPhysics::CUBE_GRAVITY * g * gravSign;
        clampVelocity(snap);

        snap.position.y += snap.velY;
        snap.position.x += GDPhysics::HORIZONTAL_SPEED;

        // Very simple ground detection: treat y ≤ 0 as ground.
        if ((snap.position.y <= 105.f && !snap.upsideDown) ||
            (snap.position.y >= 240.f * 5 && snap.upsideDown)) {
            snap.velY       = 0.f;
            snap.isOnGround = true;
        } else {
            snap.isOnGround = false;
        }
        break;
    }

    // ── Ship ──────────────────────────────────────────────────────────────────
    case 1: {
        float thrust = snap.holdingJump ? GDPhysics::SHIP_THRUST : -GDPhysics::SHIP_GRAVITY;
        thrust *= g * gravSign;
        snap.velY += thrust;
        snap.velY  = std::clamp(snap.velY,
                                GDPhysics::SHIP_MAX_DOWN,
                                GDPhysics::SHIP_MAX_UP);
        snap.position.y += snap.velY;
        snap.position.x += GDPhysics::HORIZONTAL_SPEED;
        break;
    }

    // ── Ball ──────────────────────────────────────────────────────────────────
    case 2: {
        snap.velY -= GDPhysics::BALL_GRAVITY * g * gravSign;
        clampVelocity(snap);
        snap.position.y += snap.velY;
        snap.position.x += GDPhysics::HORIZONTAL_SPEED;
        break;
    }

    // ── UFO ───────────────────────────────────────────────────────────────────
    case 3: {
        float force = snap.holdingJump ? GDPhysics::UFO_BOOST : -GDPhysics::UFO_GRAVITY;
        force *= g * gravSign;
        snap.velY += force;
        snap.velY  = std::clamp(snap.velY,
                                GDPhysics::UFO_MAX_DOWN,
                                GDPhysics::UFO_MAX_UP);
        snap.position.y += snap.velY;
        snap.position.x += GDPhysics::HORIZONTAL_SPEED;
        break;
    }

    // ── Wave ──────────────────────────────────────────────────────────────────
    case 4: {
        float dy = snap.holdingJump ? GDPhysics::WAVE_SPEED_Y : -GDPhysics::WAVE_SPEED_Y;
        dy *= gravSign;
        snap.position.y += dy;
        snap.position.x += GDPhysics::HORIZONTAL_SPEED;
        break;
    }

    // ── Robot (cube + extra jump) — approximate as cube ───────────────────────
    case 5:
    // ── Spider — approximate as ball ──────────────────────────────────────────
    case 6:
    default: {
        snap.velY -= GDPhysics::CUBE_GRAVITY * g * gravSign;
        clampVelocity(snap);
        snap.position.y += snap.velY;
        snap.position.x += GDPhysics::HORIZONTAL_SPEED;
        break;
    }
    }
}

void TrajectorySystem::clampVelocity(PhysicsSnapshot& snap) const {
    if (snap.velY < -GDPhysics::CUBE_MAX_FALL)
        snap.velY = -GDPhysics::CUBE_MAX_FALL;
}
