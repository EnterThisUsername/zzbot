#include "FPSUnlocker.hpp"

FPSUnlocker* FPSUnlocker::get() {
    static FPSUnlocker s_instance;
    return &s_instance;
}

void FPSUnlocker::setTargetFPS(float fps) {
    m_targetFPS = fps;
    apply();
    log::info("[zzBot] FPS target set to {:.1f}", fps);
}

void FPSUnlocker::apply() {
    auto* dir = CCDirector::get();
    if (!dir) return;

    if (m_targetFPS <= 0.0f) {
        // Uncapped: set to a very small interval (≈ 0 wait between frames).
        // The actual cap becomes the monitor/driver vsync or CPU limit.
        dir->setAnimationInterval(1.0 / 10000.0);
    } else {
        dir->setAnimationInterval(1.0 / static_cast<double>(m_targetFPS));
    }
}

void FPSUnlocker::reset() {
    m_targetFPS = 60.0f;
    apply();
}
