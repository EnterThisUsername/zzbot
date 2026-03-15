#pragma once
#include <Geode/Geode.hpp>
#include <vector>
#include <string>

using namespace geode::prelude;

// ─────────────────────────────────────────────────────────────────────────────
// Data types
// ─────────────────────────────────────────────────────────────────────────────

enum class BotState { Idle, Recording, Playing };

/// One recorded input event. Only transitions are stored — not every tick.
struct InputFrame {
    int  tick;      ///< Physics tick index when the input changed
    int  button;    ///< 1 = jump/up, 2 = left, 3 = right
    bool isPress;   ///< true = button pressed, false = button released
    bool player2;   ///< true = player 2 input

    bool operator<(const InputFrame& o) const { return tick < o.tick; }
};

/// A complete recorded macro.
struct Macro {
    std::string             name;
    std::vector<InputFrame> frames;
    float                   recordedTPS = 240.0f; ///< TPS active during recording
};

// ─────────────────────────────────────────────────────────────────────────────
// ReplaySystem — singleton
// ─────────────────────────────────────────────────────────────────────────────

class ReplaySystem {
public:
    static ReplaySystem* get();

    // ── State ─────────────────────────────────────────────────────────────────
    BotState state()       const { return m_state; }
    bool     isRecording() const { return m_state == BotState::Recording; }
    bool     isPlaying()   const { return m_state == BotState::Playing;   }

    // ── Recording ─────────────────────────────────────────────────────────────
    void startRecording();
    void stopRecording();
    void recordInput(int tick, int button, bool isPress, bool player2);

    // ── Playback ──────────────────────────────────────────────────────────────

    /// Begin playback. Shows a Geode notification if TPS has changed since
    /// the macro was recorded, because that would break determinism.
    void startPlayback();
    void stopPlayback();

    /// Called once per physics tick during playback.
    /// Returns true if any input was injected this tick.
    bool tickPlayback(int tick);

    // ── Persistence ───────────────────────────────────────────────────────────
    bool saveToFile(const std::string& path) const;
    bool loadFromFile(const std::string& path);

    // ── Helpers ───────────────────────────────────────────────────────────────
    void resetMacro();

    const Macro& currentMacro() const { return m_macro; }
    Macro&       currentMacro()       { return m_macro; }

    int playbackIndex() const { return m_playIndex; }

private:
    ReplaySystem() = default;

    BotState m_state     = BotState::Idle;
    Macro    m_macro;
    int      m_playIndex = 0;
};
