#pragma once
#include <Geode/Geode.hpp>
#include <deque>

using namespace geode::prelude;

// ─────────────────────────────────────────────────────────────────────────────
// HitboxViewer — singleton
//
// Renders the player's hitbox as a coloured rectangle on top of the level,
// and optionally draws a fading trail of previous hitbox positions.
//
// The actual drawing is done by VisualizerNode (CCNode subclass added to
// PlayLayer) — the HitboxViewer singleton just holds the settings and the
// trail history.
// ─────────────────────────────────────────────────────────────────────────────

struct TrailEntry {
    CCRect  rect;
    float   opacity;   ///< 1.0 → 0.0 (newest → oldest)
};

class HitboxViewer {
public:
    static HitboxViewer* get();

    // ── Settings ─────────────────────────────────────────────────────────────
    bool showHitbox()  const { return m_showHitbox;  }
    bool showTrail()   const { return m_showTrail;   }
    int  trailLength() const { return m_trailLength; }

    void setShowHitbox(bool v)  { m_showHitbox  = v; }
    void setShowTrail(bool v)   { m_showTrail   = v; }
    void setTrailLength(int n)  { m_trailLength = std::clamp(n, 1, 4800); }

    // ── Trail management ─────────────────────────────────────────────────────
    void pushTrailEntry(const CCRect& rect);
    void clearTrail();

    const std::deque<TrailEntry>& trail() const { return m_trail; }

    // ── Colours ───────────────────────────────────────────────────────────────
    ccColor4F hitboxColor()    const { return m_hitboxColor;    }
    ccColor4F innerHitboxColor() const { return m_innerColor;   }
    ccColor4F trailColor()     const { return m_trailColor;     }

private:
    HitboxViewer() = default;

    bool  m_showHitbox  = false;
    bool  m_showTrail   = false;
    int   m_trailLength = 240;

    std::deque<TrailEntry> m_trail;

    ccColor4F m_hitboxColor = { 0.0f, 0.8f, 1.0f, 0.7f };   // cyan
    ccColor4F m_innerColor  = { 1.0f, 0.2f, 0.2f, 0.7f };   // red (death box)
    ccColor4F m_trailColor  = { 1.0f, 1.0f, 0.0f, 0.5f };   // yellow trail
};
