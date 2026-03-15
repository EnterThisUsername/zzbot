#include <Geode/Geode.hpp>

#include "ReplaySystem.hpp"
#include "SpeedControl.hpp"
#include "FPSUnlocker.hpp"
#include "TPSModifier.hpp"
#include "HitboxViewer.hpp"
#include "TrajectorySystem.hpp"
#include "ClickSound.hpp"

using namespace geode::prelude;

$on_mod(Loaded) {
    auto* mod = Mod::get();

    float fps = mod->getSettingValue<double>("target-fps");
    FPSUnlocker::get()->setTargetFPS(fps);

    float tps = mod->getSettingValue<double>("target-tps");
    TPSModifier::get()->setTPS(tps);

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

    // Load click sound settings.
    ClickSound::get()->reloadFromSettings();

    log::info("[zzBot] v1.1.0 loaded.");
    log::info("[zzBot]  FPS target   : {:.0f}", fps);
    log::info("[zzBot]  TPS target   : {:.0f}", tps);
    log::info("[zzBot]  Click sounds : {}", ClickSound::get()->isEnabled());
}
