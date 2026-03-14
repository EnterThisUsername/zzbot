#pragma once
#include <Geode/Geode.hpp>
#include "TrajectorySystem.hpp"
#include "HitboxViewer.hpp"

using namespace geode::prelude;

// ─────────────────────────────────────────────────────────────────────────────
// VisualizerNode
//
// Added as a child of PlayLayer at z-order 1000 (renders above level objects).
// Every render frame it draws:
//  1. Hitbox trail   — fading rectangles from HitboxViewer::trail()
//  2. Current hitbox — live player hitbox outline
//  3. Trajectory     — dot sequence from TrajectorySystem::simulate()
// ─────────────────────────────────────────────────────────────────────────────

class VisualizerNode : public CCNode {
public:
    static VisualizerNode* create();

    bool init() override;
    void draw() override;
    void update(float dt) override;

    /// Call once per physics tick to record the hitbox position.
    void onPhysicsTick();

private:
    CCDrawNode* m_draw = nullptr;

    void drawRect(const CCRect& r, const ccColor4F& fill, const ccColor4F& border);
    void drawTrajectory(const std::vector<CCPoint>& pts);
};
