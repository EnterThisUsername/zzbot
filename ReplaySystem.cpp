#include "ReplaySystem.hpp"
#include "TPSModifier.hpp"
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
    m_macro.name        = "unnamed";
    m_macro.recordedTPS = TPSModifier::get()->tps();   // snapshot current TPS
    m_state             = BotState::Recording;
    log::info("[zzBot] Recording started (TPS: {:.1f}).", m_macro.recordedTPS);
}

void ReplaySystem::stopRecording() {
    m_state = BotState::Idle;
    log::info("[zzBot] Recording stopped. {} frames captured.",
              m_macro.frames.size());
}

void ReplaySystem::recordInput(int tick, int button, bool isPress, bool player2) {
    if (m_state != BotState::Recording) return;

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
        log::warn("[zzBot] No macro loaded.");
        return;
    }

    // ── TPS mismatch warning ──────────────────────────────────────────────────
    float currentTPS  = TPSModifier::get()->tps();
    float recordedTPS = m_macro.recordedTPS;

    if (std::abs(currentTPS - recordedTPS) > 0.5f) {
        // Show a persistent Geode notification so the user can't miss it.
        Notification::create(
            fmt::format(
                "TPS mismatch! Recorded at {:.0f} TPS, currently {:.0f} TPS. "
                "Replay may desync.",
                recordedTPS, currentTPS
            ),
            NotificationIcon::Warning,
            5.0f
        )->show();

        log::warn("[zzBot] TPS mismatch: recorded={:.1f}, current={:.1f}",
                  recordedTPS, currentTPS);
    }

    m_playIndex = 0;
    m_state     = BotState::Playing;
    log::info("[zzBot] Playback started ({} frames).", m_macro.frames.size());
}

void ReplaySystem::stopPlayback() {
    m_state = BotState::Idle;
    log::info("[zzBot] Playback stopped at index {}.", m_playIndex);
}

bool ReplaySystem::tickPlayback(int tick) {
    if (m_state != BotState::Playing) return false;

    bool injected = false;

    while (m_playIndex < static_cast<int>(m_macro.frames.size()) &&
           m_macro.frames[m_playIndex].tick <= tick)
    {
        const InputFrame& f = m_macro.frames[m_playIndex];

        auto* pl = PlayLayer::get();
        if (pl) {
            if (f.isPress)
                pl->handleButton(true,  f.button, !f.player2);
            else
                pl->handleButton(false, f.button, !f.player2);
        }

        ++m_playIndex;
        injected = true;
    }

    if (m_playIndex >= static_cast<int>(m_macro.frames.size()))
        stopPlayback();

    return injected;
}

// ── Persistence ───────────────────────────────────────────────────────────────

bool ReplaySystem::saveToFile(const std::string& path) const {
    try {
        json root;
        root["name"]        = m_macro.name;
        root["recordedTPS"] = m_macro.recordedTPS;
        root["frames"]      = json::array();

        for (const auto& f : m_macro.frames) {
            root["frames"].push_back({
                {"tick",   f.tick},
                {"button", f.button},
                {"press",  f.isPress},
                {"p2",     f.player2}
            });
        }

        std::ofstream ofs(path);
        if (!ofs.is_open()) return false;
        ofs << root.dump(2);
        log::info("[zzBot] Macro saved to '{}'.", path);
        return true;
    } catch (const std::exception& e) {
        log::error("[zzBot] Save failed: {}", e.what());
        return false;
    }
}

bool ReplaySystem::loadFromFile(const std::string& path) {
    try {
        std::ifstream ifs(path);
        if (!ifs.is_open()) return false;

        json root = json::parse(ifs);
        m_macro.frames.clear();
        m_macro.name        = root.value("name", "loaded");
        m_macro.recordedTPS = root.value("recordedTPS", 240.0f);

        for (const auto& j : root["frames"]) {
            m_macro.frames.push_back({
                j.at("tick").get<int>(),
                j.at("button").get<int>(),
                j.at("press").get<bool>(),
                j.at("p2").get<bool>()
            });
        }

        std::sort(m_macro.frames.begin(), m_macro.frames.end());

        log::info("[zzBot] Macro '{}' loaded ({} frames, recorded TPS: {:.1f}).",
                  m_macro.name, m_macro.frames.size(), m_macro.recordedTPS);
        return true;
    } catch (const std::exception& e) {
        log::error("[zzBot] Load failed: {}", e.what());
        return false;
    }
}

void ReplaySystem::resetMacro() {
    m_state = BotState::Idle;
    m_macro.frames.clear();
    m_playIndex = 0;
}
