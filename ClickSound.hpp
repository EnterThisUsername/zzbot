#pragma once
#include <Geode/Geode.hpp>

using namespace geode::prelude;

// ─────────────────────────────────────────────────────────────────────────────
// ClickSound — singleton
//
// Plays a configurable sound at the exact tick a button is pressed or
// released during playback.
//
// Sounds are resolved relative to the mod's own resources folder so the user
// can drop custom .ogg files there without needing an absolute path.
//
// FMODAudioEngine::sharedEngine()->playEffect() is used for low-latency
// one-shot playback, which is the same path GD uses for its own SFX.
// ─────────────────────────────────────────────────────────────────────────────
class ClickSound {
public:
    static ClickSound* get();

    bool isEnabled()   const { return m_enabled; }
    void setEnabled(bool v)  { m_enabled = v; }

    void setPressSound(const std::string& filename)   { m_pressFile   = filename; }
    void setReleaseSound(const std::string& filename) { m_releaseFile = filename; }

    const std::string& pressFile()   const { return m_pressFile;   }
    const std::string& releaseFile() const { return m_releaseFile; }

    /// Call this when a press input is injected during playback.
    void playPress();

    /// Call this when a release input is injected during playback.
    void playRelease();

    /// Reload filenames from mod settings (called on startup and from UI).
    void reloadFromSettings();

private:
    ClickSound() = default;

    void playFile(const std::string& filename);

    bool        m_enabled     = false;
    std::string m_pressFile   = "click_press.ogg";
    std::string m_releaseFile = "click_release.ogg";
};
