#pragma once
#include <Geode/Geode.hpp>
#include <Geode/ui/Popup.hpp>

using namespace geode::prelude;

// ─────────────────────────────────────────────────────────────────────────────
// ControlPanel — the mod's main UI window
//
// Opens via the pause menu button.  Provides controls for:
//  • Recording / playback / stop
//  • Save / Load macro from file
//  • Speed slider  (0.01 – 2.0)
//  • FPS number field
//  • TPS number field
//  • Hitbox toggle + trail toggle
//  • Trajectory toggle
//  • Frame stepper  (enter/exit; X/C/V are keyboard shortcuts)
//
// Implemented as a Geode Popup<> so it renders correctly on all platforms
// and respects the pause menu layering.
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
    CCLabelTTF* makeLabel(const char* text, float fontSize, ccColor3B col = ccWHITE);

    ButtonSprite* makeButtonSprite(const char* label,
                                   ccColor3B bg = { 60, 60, 90 });

    CCMenuItemSpriteExtra* makeButton(const char* label,
                                      SEL_MenuHandler handler,
                                      ccColor3B bg = { 60, 60, 90 });

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

    // ── Status label updated by timer ─────────────────────────────────────────
    void refreshStatus(float);
    CCLabelTTF* m_statusLabel = nullptr;

    // Speed / FPS / TPS input fields (simple text labels for now;
    // full editing delegated to InputSettingNode in the Geode settings UI)
    CCLabelTTF* m_speedLabel   = nullptr;
    CCLabelTTF* m_fpsLabel     = nullptr;
    CCLabelTTF* m_tpsLabel     = nullptr;
};
