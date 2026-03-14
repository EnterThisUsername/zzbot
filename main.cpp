#include <Geode/Geode.hpp>

#include "ReplaySystem.hpp"
#include "SpeedControl.hpp"
#include "FPSUnlocker.hpp"
#include "TPSModifier.hpp"
#include "HitboxViewer.hpp"
#include "TrajectorySystem.hpp"

using namespace geode::prelude;

// ─────────────────────────────────────────────────────────────────────────────
// $on_mod(Loaded) — runs once when the mod is fully loaded into the process.
// Reads mod.json settings and initialises every subsystem with the persisted
// values so the user's configuration survives game restarts.
// ─────────────────────────────────────────────────────────────────────────────

$on_mod(Loaded) {
    auto* mod = Mod::get();

    // ── FPS ───────────────────────────────────────────────────────────────────
    float fps = mod->getSettingValue<double>("target-fps");
    FPSUnlocker::get()->setTargetFPS(fps);

    // ── TPS ───────────────────────────────────────────────────────────────────
    float tps = mod->getSettingValue<double>("target-tps");
    TPSModifier::get()->setTPS(tps);

    // ── Speed ─────────────────────────────────────────────────────────────────
    // Speed is intentionally NOT applied here — it should only take effect
    // inside an active PlayLayer session.  Applying it globally would slow
    // the menus too.

    // ── Hitbox / Trail / Trajectory ───────────────────────────────────────────
    HitboxViewer::get()->setShowHitbox(
        mod->getSettingValue<bool>("show-hitboxes"));
    HitboxViewer::get()->setShowTrail(
        mod->getSettingValue<bool>("show-hitbox-trail"));
    HitboxViewer::get()->setTrailLength(
        static_cast<int>(mod->getSettingValue<int64_t>("hitbox-trail-length")));

    TrajectorySystem::get()->setEnabled(
        mod->getSettingValue<bool>("show-trajectory"));
    TrajectorySystem::get()->setLookaheadTicks(
        static_cast<int>(mod->getSettingValue<int64_t>("trajectory-steps")));

    log::info("[BotToolkit] Mod loaded successfully.");
    log::info("[BotToolkit]  FPS target : {:.0f}", fps);
    log::info("[BotToolkit]  TPS target : {:.0f}", tps);
    log::info("[BotToolkit]  Hitboxes   : {}", HitboxViewer::get()->showHitbox());
    log::info("[BotToolkit]  Trajectory : {}", TrajectorySystem::get()->isEnabled());
}
