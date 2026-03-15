#include "ClickSound.hpp"

ClickSound* ClickSound::get() {
    static ClickSound s_instance;
    return &s_instance;
}

void ClickSound::reloadFromSettings() {
    auto* mod   = Mod::get();
    m_enabled   = mod->getSettingValue<bool>("click-sound-enabled");
    m_pressFile = mod->getSettingValue<std::string>("press-sound");
    m_releaseFile = mod->getSettingValue<std::string>("release-sound");
}

void ClickSound::playPress() {
    if (!m_enabled) return;
    playFile(m_pressFile);
}

void ClickSound::playRelease() {
    if (!m_enabled) return;
    playFile(m_releaseFile);
}

void ClickSound::playFile(const std::string& filename) {
    if (filename.empty()) return;

    // Resolve the file relative to the mod's own resources directory.
    // Mod::get()->getResourcesDir() returns the path that Geode unpacks the
    // mod's bundled files into, so users can place custom sounds there.
    auto fullPath = Mod::get()->getResourcesDir() / filename;

    if (!std::filesystem::exists(fullPath)) {
        log::warn("[zzBot] Click sound file not found: {}", fullPath.string());
        return;
    }

    FMODAudioEngine::sharedEngine()->playEffect(fullPath.string());
}
