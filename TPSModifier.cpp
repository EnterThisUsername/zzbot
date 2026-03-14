#include "TPSModifier.hpp"
#include <algorithm>

TPSModifier* TPSModifier::get() {
    static TPSModifier s_instance;
    return &s_instance;
}

void TPSModifier::setTPS(float tps) {
    tps     = std::clamp(tps, 1.0f, 10000.0f);
    m_tps   = tps;
    m_step  = 1.0f / tps;
    log::info("[zzBot] TPS set to {:.1f} (step = {:.6f}s)", m_tps, m_step);
}
