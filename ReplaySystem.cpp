#include "ReplaySystem.hpp"
#include <fstream>
#include <algorithm>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// ─────────────────────────────────────────────────────────────────────────────
ReplaySystem* ReplaySystem::get() {
    static ReplaySystem s_instance;
    return &s_instance;
}

// ── Recording ─────────────────────────────────────────────────────────────────

void ReplaySystem::startRecording() {
    m_macro.frames.clear();
    m_macro.name = "unnamed";
    m_state      = BotState::Recording;
    log::info("[BotToolkit] Recording started.");
}

void ReplaySystem::stopRecording() {
    m_state = BotState::Idle;
    log::info("[BotToolkit] Recording stopped. {} frames captured.",
              m_macro.frames.size());
}

void ReplaySystem::recordInput(int tick, int button, bool isPress, bool player2) {
    if (m_state != BotState::Recording) return;

    // De-duplicate: if the last frame for this button+player already has the
    // same state, skip it (handles redundant input events).
    if (!m_macro.frames.empty()) {
        const auto& last = m_macro.frames.back();
        if (last.tick == tick && last.button == button &&
            last.isPress == isPress && last.player2 == player2)
            return;
    }

    m_macro.frames.push_back({ tick, button, isPress, player2 });
}

// ── Playback ──────────────────────────────────────────────────────────────────

void ReplaySystem::startPlayback() {
    if (m_macro.frames.empty()) {
        log::warn("[BotToolkit] No macro loaded.");
        return;
    }
    m_playIndex = 0;
    m_state     = BotState::Playing;
    log::info("[BotToolkit] Playback started ({} frames).",
              m_macro.frames.size());
}

void ReplaySystem::stopPlayback() {
    m_state = BotState::Idle;
    log::info("[BotToolkit] Playback stopped at index {}.", m_playIndex);
}

bool ReplaySystem::tickPlayback(int tick) {
    if (m_state != BotState::Playing) return false;

    bool injected = false;

    // Process all frames scheduled at or before this tick.
    while (m_playIndex < static_cast<int>(m_macro.frames.size()) &&
           m_macro.frames[m_playIndex].tick <= tick)
    {
        const InputFrame& f = m_macro.frames[m_playIndex];

        // Retrieve the active PlayLayer and inject the stored input.
        auto* pl = PlayLayer::get();
        if (pl) {
            auto* player = f.player2 ? pl->m_player2 : pl->m_player1;
            if (player) {
                if (f.isPress)
                    pl->handleButton(true, f.button, !f.player2);
                else
                    pl->handleButton(false, f.button, !f.player2);
            }
        }

        ++m_playIndex;
        injected = true;
    }

    // End naturally when all inputs have been replayed.
    if (m_playIndex >= static_cast<int>(m_macro.frames.size()))
        stopPlayback();

    return injected;
}

// ── Persistence ───────────────────────────────────────────────────────────────

bool ReplaySystem::saveToFile(const std::string& path) const {
    try {
        json root;
        root["name"]   = m_macro.name;
        root["frames"] = json::array();

        for (const auto& f : m_macro.frames) {
            root["frames"].push_back({
                {"tick",    f.tick},
                {"button",  f.button},
                {"press",   f.isPress},
                {"p2",      f.player2}
            });
        }

        std::ofstream ofs(path);
        if (!ofs.is_open()) return false;
        ofs << root.dump(2);
        log::info("[BotToolkit] Macro saved to '{}'.", path);
        return true;
    } catch (const std::exception& e) {
        log::error("[BotToolkit] Save failed: {}", e.what());
        return false;
    }
}

bool ReplaySystem::loadFromFile(const std::string& path) {
    try {
        std::ifstream ifs(path);
        if (!ifs.is_open()) return false;

        json root = json::parse(ifs);
        m_macro.frames.clear();
        m_macro.name = root.value("name", "loaded");

        for (const auto& j : root["frames"]) {
            m_macro.frames.push_back({
                j.at("tick").get<int>(),
                j.at("button").get<int>(),
                j.at("press").get<bool>(),
                j.at("p2").get<bool>()
            });
        }

        // Ensure frames are ordered by tick (in case of manual edits).
        std::sort(m_macro.frames.begin(), m_macro.frames.end());

        log::info("[BotToolkit] Macro '{}' loaded ({} frames).",
                  m_macro.name, m_macro.frames.size());
        return true;
    } catch (const std::exception& e) {
        log::error("[BotToolkit] Load failed: {}", e.what());
        return false;
    }
}

void ReplaySystem::resetMacro() {
    m_state = BotState::Idle;
    m_macro.frames.clear();
    m_playIndex = 0;
}
