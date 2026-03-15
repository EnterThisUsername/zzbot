#pragma once
#include <Geode/Geode.hpp>
#include <Geode/ui/Popup.hpp>
#include <Geode/utils/file.hpp>
#include <filesystem>

using namespace geode::prelude;

class ControlPanel : public Popup<> {
public:
    static ControlPanel* create();

protected:
    bool setup() override;

private:
    CCNode* buildReplaySection();
    CCNode* buildSpeedSection();
    CCNode* buildPerfSection();
    CCNode* buildVisualSection();
    CCNode* buildStepperSection();

    CCLabelBMFont*         makeLabel(const char* text, float fontSize, ccColor3B col = ccWHITE);
    CCMenuItemSpriteExtra* makeButton(const char* label, SEL_MenuHandler handler);

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
    void onToggleClickSound(CCObject*);

    void refreshStatus(float);

    CCLabelBMFont* m_statusLabel = nullptr;
    CCLabelBMFont* m_speedLabel  = nullptr;
    CCLabelBMFont* m_fpsLabel    = nullptr;
    CCLabelBMFont* m_tpsLabel    = nullptr;

    using PickTask = Task<Result<std::filesystem::path>>;
    EventListener<PickTask> m_saveListener;
    EventListener<PickTask> m_loadListener;
};
