#pragma once
#include <Geode/Geode.hpp>
#include <Geode/ui/Popup.hpp>
#include <Geode/utils/file.hpp>
#include <filesystem>

using namespace geode::prelude;

// ─────────────────────────────────────────────────────────────────────────────
// ControlPanel — the mod's main UI window
//
// Opens via the pause menu button.  Provides controls for:
//  • Recording / playback / stop
//  • Save / Load macro from file
//  • Speed presets  (0.01 – 2.0)
//  • FPS presets
//  • Hitbox toggle + trail toggle
//  • Trajectory toggle
//  • Frame stepper  (enter/exit; X/C/V are keyboard shortcuts)
// ─────────────────────────────────────────────────────────────────────────────

class ControlPanel : public Popup<> {
public:
    static ControlPanel* create();

protected:
    bool setup() override;

private:
    // ── Section builders ──────────────────────────────────────────────────────
    CCNode* buildReplaySection();
    CCNode* buildSpeedSection();
    CCNode* buildPerfSection();
    CCNode* buildVisualSection();
    CCNode* buildStepperSection();

    // ── UI helpers ────────────────────────────────────────────────────────────

    /// Create a BMFont label scaled to approximate the requested pixel size.
    /// bigFont.fnt native height ≈ 57 px → scale = size / 57.
    CCLabelBMFont* makeLabel(const char* text, float fontSize, ccColor3B col = ccWHITE);

    CCMenuItemSpriteExtra* makeButton(const char* label, SEL_MenuHandler handler);

    // ── Callbacks ─────────────────────────────────────────────────────────────
    void onRecord(CCObject*);
    void onPlay(CCObject*);
    void onStop(CCObject*);
    void onSave(CCObject*);
    void onLoad(CCObject*);

    void onEnterStepper(CCObject*);
    void onExitStepper(CCObject*);

    void onToggleHitbox(CCObject*);
    void onToggleTrail(CCObject*);
    void onToggleTrajectory(CCObject*);

    // ── Status / info labels ──────────────────────────────────────────────────
    void refreshStatus(float);
    CCLabelBMFont* m_statusLabel = nullptr;
    CCLabelBMFont* m_speedLabel  = nullptr;
    CCLabelBMFont* m_fpsLabel    = nullptr;
    CCLabelBMFont* m_tpsLabel    = nullptr;

    // ── File-picker Task listeners ────────────────────────────────────────────
    // Geode 5.x uses a Task-based async API; EventListeners must stay alive
    // for the duration of the native OS file dialog.
    using PickTask = Task<Result<std::filesystem::path>>;
    EventListener<PickTask> m_saveListener;
    EventListener<PickTask> m_loadListener;
};
