#pragma once
#include <Geode/Geode.hpp>

using namespace geode::prelude;

// ─────────────────────────────────────────────────────────────────────────────
// FPSUnlocker — singleton
//
// Geometry Dash caps the render loop at 60 FPS via
//   CCDirector::setAnimationInterval(1.0 / 60.0)
//
// This class stores and applies a custom animation interval so the engine
// can run at any desired frame rate.  Hooks in the PlayLayer modify hook
// prevent GD from resetting the interval back to 60 during level entry.
// ─────────────────────────────────────────────────────────────────────────────
class FPSUnlocker {
public:
    static FPSUnlocker* get();

    /// Set the target FPS (0 = uncapped — runs as fast as possible).
    void setTargetFPS(float fps);

    float targetFPS() const { return m_targetFPS; }

    /// Apply the currently stored target to CCDirector.
    void apply();

    /// Restore the stock 60 FPS cap.
    void reset();

private:
    FPSUnlocker()   = default;
    float m_targetFPS = 240.0f;
};
