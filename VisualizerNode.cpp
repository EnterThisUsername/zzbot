#include "VisualizerNode.hpp"
#include "TPSModifier.hpp"

VisualizerNode* VisualizerNode::create() {
    auto* node = new (std::nothrow) VisualizerNode();
    if (node && node->init()) {
        node->autorelease();
        return node;
    }
    CC_SAFE_DELETE(node);
    return nullptr;
}

bool VisualizerNode::init() {
    if (!CCNode::init()) return false;

    m_draw = CCDrawNode::create();
    this->addChild(m_draw);

    this->scheduleUpdate();
    this->setZOrder(1000);

    return true;
}

// ─────────────────────────────────────────────────────────────────────────────
// Called once per physics tick by the PlayLayer hook.
// Snapshot the player hitbox for the trail.
// ─────────────────────────────────────────────────────────────────────────────
void VisualizerNode::onPhysicsTick() {
    auto* pl = PlayLayer::get();
    if (!pl || !pl->m_player1) return;

    if (HitboxViewer::get()->showTrail()) {
        CCRect hb = pl->m_player1->getObjectRect();
        HitboxViewer::get()->pushTrailEntry(hb);
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// update — runs every render frame
// ─────────────────────────────────────────────────────────────────────────────
void VisualizerNode::update(float /*dt*/) {
    m_draw->clear();

    auto* pl = PlayLayer::get();
    if (!pl || !pl->m_player1) return;

    auto* hv  = HitboxViewer::get();
    auto* traj = TrajectorySystem::get();

    // ── Hitbox trail ─────────────────────────────────────────────────────────
    if (hv->showTrail()) {
        for (const auto& entry : hv->trail()) {
            ccColor4F c = hv->trailColor();
            c.a *= entry.opacity;
            drawRect(entry.rect, c, { c.r, c.g, c.b, c.a * 1.2f });
        }
    }

    // ── Live hitbox ───────────────────────────────────────────────────────────
    if (hv->showHitbox()) {
        CCRect outer = pl->m_player1->getObjectRect();
        drawRect(outer,
                 hv->hitboxColor(),
                 { 1.0f, 1.0f, 1.0f, 0.9f });
    }

    // ── Trajectory ────────────────────────────────────────────────────────────
    if (traj->isEnabled()) {
        float step = TPSModifier::get()->physicsStep();
        auto pts   = traj->simulate(pl->m_player1, step);

        // Convert from world → local space (VisualizerNode follows PlayLayer).
        // PlayLayer offsets the world by its position; we must counteract that.
        drawTrajectory(pts);
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// draw — Cocos2d draw hook (called after update in the same frame)
// ─────────────────────────────────────────────────────────────────────────────
void VisualizerNode::draw() {
    CCNode::draw();
    // CCDrawNode handles its own GL calls; nothing extra needed here.
}

// ─────────────────────────────────────────────────────────────────────────────
// Helpers
// ─────────────────────────────────────────────────────────────────────────────

void VisualizerNode::drawRect(const CCRect& r,
                              const ccColor4F& fill,
                              const ccColor4F& border)
{
    CCPoint verts[4] = {
        { r.getMinX(), r.getMinY() },
        { r.getMaxX(), r.getMinY() },
        { r.getMaxX(), r.getMaxY() },
        { r.getMinX(), r.getMaxY() }
    };

    m_draw->drawPolygon(verts, 4, fill, 0.5f, border);
}

void VisualizerNode::drawTrajectory(const std::vector<CCPoint>& pts) {
    if (pts.size() < 2) return;

    // Draw as a series of dots, fading from solid to transparent.
    const size_t n = pts.size();
    for (size_t i = 0; i < n; ++i) {
        float t      = 1.0f - static_cast<float>(i) / static_cast<float>(n);
        float radius = 1.5f + t * 1.0f;
        ccColor4F c  = { 0.0f, 1.0f, 0.4f, t * 0.8f };
        m_draw->drawDot(pts[i], radius, c);
    }
}
