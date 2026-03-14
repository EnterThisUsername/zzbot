#include "HitboxViewer.hpp"

HitboxViewer* HitboxViewer::get() {
    static HitboxViewer s_instance;
    return &s_instance;
}

void HitboxViewer::pushTrailEntry(const CCRect& rect) {
    if (!m_showTrail) return;

    // Recalculate opacity for all existing entries (fade older ones).
    const int len = m_trailLength;

    m_trail.push_front({ rect, 1.0f });

    // Trim to max length.
    while (static_cast<int>(m_trail.size()) > len)
        m_trail.pop_back();

    // Linearly distribute opacity: newest = 1.0, oldest ≈ 0.
    for (int i = 0; i < static_cast<int>(m_trail.size()); ++i) {
        m_trail[static_cast<size_t>(i)].opacity =
            1.0f - static_cast<float>(i) / static_cast<float>(len);
    }
}

void HitboxViewer::clearTrail() {
    m_trail.clear();
}
