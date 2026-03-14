#include <Geode/Geode.hpp>
#include <Geode/modify/CCDirector.hpp>
#include "../FPSUnlocker.hpp"

using namespace geode::prelude;

// ─────────────────────────────────────────────────────────────────────────────
// CCDirector hook — prevents GD from resetting setAnimationInterval to 1/60
//
// GD calls setAnimationInterval(1.0/60.0) in several places (scene transitions,
// level loads, etc.).  We intercept every call and silently replace the value
// with the user's chosen target.
// ─────────────────────────────────────────────────────────────────────────────

class $modify(BotCCDirector, CCDirector) {

    void setAnimationInterval(double interval) {
        auto* unlocker = FPSUnlocker::get();
        float target   = unlocker->targetFPS();

        double desiredInterval = (target <= 0.f)
            ? (1.0 / 10000.0)                  // uncapped
            : (1.0 / static_cast<double>(target));

        // Only override when the caller is trying to set 60 FPS (the GD default).
        // This lets the engine set very long intervals (e.g. during background
        // transitions) without us clobbering them.
        constexpr double kGDDefault = 1.0 / 60.0;
        constexpr double kEpsilon   = 0.00001;

        if (std::abs(interval - kGDDefault) < kEpsilon)
            CCDirector::setAnimationInterval(desiredInterval);
        else
            CCDirector::setAnimationInterval(interval);
    }
};
