#include "ControlPanel.hpp"
#include "ReplaySystem.hpp"
#include "SpeedControl.hpp"
#include "FPSUnlocker.hpp"
#include "TPSModifier.hpp"
#include "FrameStepper.hpp"
#include "HitboxViewer.hpp"
#include "TrajectorySystem.hpp"

#include <Geode/ui/TextInput.hpp>
#include <Geode/utils/file.hpp>

// ─────────────────────────────────────────────────────────────────────────────

static constexpr float PANEL_W = 460.f;
static constexpr float PANEL_H = 340.f;

// ─────────────────────────────────────────────────────────────────────────────

ControlPanel* ControlPanel::create() {
    auto* p = new (std::nothrow) ControlPanel();
    if (p && p->initAnchored(PANEL_W, PANEL_H)) {
        p->autorelease();
        return p;
    }
    CC_SAFE_DELETE(p);
    return nullptr;
}

// ─────────────────────────────────────────────────────────────────────────────
// Helpers
// ─────────────────────────────────────────────────────────────────────────────

CCLabelTTF* ControlPanel::makeLabel(const char* text, float fontSize, ccColor3B col) {
    auto* lbl = CCLabelTTF::create(text, "bigFont.fnt", fontSize);
    if (lbl) lbl->setColor(col);
    return lbl;
}

CCMenuItemSpriteExtra* ControlPanel::makeButton(const char* label,
                                                 SEL_MenuHandler handler,
                                                 ccColor3B bg)
{
    auto* spr = ButtonSprite::create(label, "goldFont.fnt", "GJ_button_04.png", 0.8f);
    return CCMenuItemSpriteExtra::create(spr, this, handler);
}

// ─────────────────────────────────────────────────────────────────────────────
// setup
// ─────────────────────────────────────────────────────────────────────────────

bool ControlPanel::setup() {
    this->setTitle("zzBot");

    // ── Background ─────────────────────────────────────────────────────────
    auto* bg = CCLayerColor::create({ 18, 18, 32, 230 }, PANEL_W - 8, PANEL_H - 40);
    bg->setPosition({ 4, 4 });
    m_mainLayer->addChild(bg, -1);

    // ── Sections ───────────────────────────────────────────────────────────
    float yOff = PANEL_H - 56.f;
    float xPad = 14.f;

    auto addSection = [&](CCNode* sec, float height) {
        if (!sec) return;
        sec->setPosition({ xPad, yOff - height });
        m_mainLayer->addChild(sec);
        yOff -= (height + 4.f);
    };

    addSection(buildReplaySection(),  54.f);
    addSection(buildSpeedSection(),   38.f);
    addSection(buildPerfSection(),    38.f);
    addSection(buildVisualSection(),  38.f);
    addSection(buildStepperSection(), 38.f);

    // ── Status label ───────────────────────────────────────────────────────
    m_statusLabel = makeLabel("Status: Idle", 9.f, { 180, 255, 180 });
    m_statusLabel->setAnchorPoint({ 0.f, 0.f });
    m_statusLabel->setPosition({ xPad, 10.f });
    m_mainLayer->addChild(m_statusLabel);

    this->schedule(schedule_selector(ControlPanel::refreshStatus), 0.15f);
    return true;
}

// ─────────────────────────────────────────────────────────────────────────────
// Section builders
// ─────────────────────────────────────────────────────────────────────────────

CCNode* ControlPanel::buildReplaySection() {
    auto* node = CCNode::create();
    auto* menu = CCMenu::create();
    menu->setPosition(CCPointZero);

    float x = 0.f;
    auto place = [&](CCMenuItemSpriteExtra* btn) {
        btn->setPosition({ x + btn->getContentSize().width * 0.5f, 16.f });
        menu->addChild(btn);
        x += btn->getContentSize().width + 6.f;
    };

    place(makeButton("Record", menu_selector(ControlPanel::onRecord)));
    place(makeButton("Play",   menu_selector(ControlPanel::onPlay)));
    place(makeButton("Stop",   menu_selector(ControlPanel::onStop)));
    place(makeButton("Save",   menu_selector(ControlPanel::onSave)));
    place(makeButton("Load",   menu_selector(ControlPanel::onLoad)));

    auto* hdr = makeLabel("Replay", 10.f, { 200, 200, 255 });
    hdr->setAnchorPoint({ 0.f, 1.f });
    hdr->setPosition({ 0.f, 46.f });
    node->addChild(hdr);
    node->addChild(menu);
    return node;
}

CCNode* ControlPanel::buildSpeedSection() {
    auto* node = CCNode::create();

    auto* hdr = makeLabel("Speed", 10.f, { 200, 200, 255 });
    hdr->setAnchorPoint({ 0.f, 1.f });
    hdr->setPosition({ 0.f, 32.f });
    node->addChild(hdr);

    // Speed preset buttons
    auto* menu = CCMenu::create();
    menu->setPosition(CCPointZero);

    struct Preset { const char* lbl; float val; };
    static const Preset presets[] = {
        {"0.01x", 0.01f}, {"0.1x", 0.1f}, {"0.25x", 0.25f},
        {"0.5x",  0.5f},  {"1.0x", 1.0f}, {"2.0x",  2.0f}
    };

    float x = 0.f;
    for (const auto& p : presets) {
        auto* spr = ButtonSprite::create(p.lbl, "bigFont.fnt", "GJ_button_06.png", 0.65f);
        auto* btn = CCMenuItemSpriteExtra::create(spr, this,
            [](CCObject* sender) {
                // Speed is stored in the button's tag as int (val * 1000)
                float spd = static_cast<float>(
                    static_cast<CCNode*>(sender)->getTag()) / 1000.f;
                SpeedControl::get()->setSpeed(spd);
                Mod::get()->setSavedValue("speed-multiplier", spd);
            });
        btn->setTag(static_cast<int>(p.val * 1000.f));
        btn->setPosition({ x + spr->getContentSize().width * 0.5f, 14.f });
        menu->addChild(btn);
        x += spr->getContentSize().width + 4.f;
    }

    node->addChild(menu);

    m_speedLabel = makeLabel(
        fmt::format("Current: {:.2f}x", SpeedControl::get()->speed()).c_str(),
        8.f, { 220, 220, 220 });
    m_speedLabel->setAnchorPoint({ 0.f, 0.f });
    m_speedLabel->setPosition({ x + 6.f, 8.f });
    node->addChild(m_speedLabel);

    return node;
}

CCNode* ControlPanel::buildPerfSection() {
    auto* node = CCNode::create();

    auto* hdr = makeLabel("Performance", 10.f, { 200, 200, 255 });
    hdr->setAnchorPoint({ 0.f, 1.f });
    hdr->setPosition({ 0.f, 32.f });
    node->addChild(hdr);

    auto* menu = CCMenu::create();
    menu->setPosition(CCPointZero);

    // FPS presets
    struct FPSPreset { const char* lbl; float fps; };
    static const FPSPreset fpsP[] = {
        {"60",60.f},{"120",120.f},{"240",240.f},{"480",480.f},{"Unlim",0.f}
    };
    float x = 0.f;
    for (const auto& p : fpsP) {
        auto* spr = ButtonSprite::create(p.lbl, "bigFont.fnt", "GJ_button_06.png", 0.65f);
        auto* btn = CCMenuItemSpriteExtra::create(spr, this,
            [](CCObject* sender) {
                float fps = static_cast<float>(
                    static_cast<CCNode*>(sender)->getTag());
                FPSUnlocker::get()->setTargetFPS(fps);
                Mod::get()->setSavedValue("target-fps", fps);
            });
        btn->setTag(static_cast<int>(p.fps));
        btn->setPosition({ x + spr->getContentSize().width * 0.5f, 14.f });
        menu->addChild(btn);
        x += spr->getContentSize().width + 4.f;
    }

    // TPS label
    m_fpsLabel = makeLabel(
        fmt::format("FPS: {:.0f}", FPSUnlocker::get()->targetFPS()).c_str(),
        8.f, { 220, 220, 220 });
    m_fpsLabel->setAnchorPoint({ 0.f, 0.f });
    m_fpsLabel->setPosition({ x + 6.f, 14.f });
    node->addChild(m_fpsLabel);

    node->addChild(menu);
    return node;
}

CCNode* ControlPanel::buildVisualSection() {
    auto* node = CCNode::create();

    auto* hdr = makeLabel("Visuals", 10.f, { 200, 200, 255 });
    hdr->setAnchorPoint({ 0.f, 1.f });
    hdr->setPosition({ 0.f, 32.f });
    node->addChild(hdr);

    auto* menu = CCMenu::create();
    menu->setPosition(CCPointZero);

    struct Toggle { const char* lbl; SEL_MenuHandler sel; };
    Toggle toggles[] = {
        {"Hitboxes",    menu_selector(ControlPanel::onToggleHitbox)},
        {"HB Trail",    menu_selector(ControlPanel::onToggleTrail)},
        {"Trajectory",  menu_selector(ControlPanel::onToggleTrajectory)},
    };

    float x = 0.f;
    for (const auto& t : toggles) {
        auto* btn = makeButton(t.lbl, t.sel);
        btn->setPosition({ x + btn->getContentSize().width * 0.5f, 14.f });
        menu->addChild(btn);
        x += btn->getContentSize().width + 6.f;
    }

    node->addChild(menu);
    return node;
}

CCNode* ControlPanel::buildStepperSection() {
    auto* node = CCNode::create();

    auto* hdr = makeLabel("Frame Stepper  [X=fwd  C=back  V=exit]", 9.f, { 200, 200, 255 });
    hdr->setAnchorPoint({ 0.f, 1.f });
    hdr->setPosition({ 0.f, 32.f });
    node->addChild(hdr);

    auto* menu = CCMenu::create();
    menu->setPosition(CCPointZero);

    auto* enterBtn = makeButton("Enter Stepper", menu_selector(ControlPanel::onEnterStepper));
    enterBtn->setPosition({ enterBtn->getContentSize().width * 0.5f, 14.f });
    menu->addChild(enterBtn);

    auto* exitBtn = makeButton("Exit Stepper", menu_selector(ControlPanel::onExitStepper));
    exitBtn->setPosition({
        enterBtn->getContentSize().width + exitBtn->getContentSize().width * 0.5f + 6.f,
        14.f
    });
    menu->addChild(exitBtn);

    node->addChild(menu);
    return node;
}

// ─────────────────────────────────────────────────────────────────────────────
// Callbacks
// ─────────────────────────────────────────────────────────────────────────────

void ControlPanel::onRecord(CCObject*) {
    auto* rs = ReplaySystem::get();
    if (!rs->isRecording()) {
        rs->resetMacro();
        rs->startRecording();
    }
}

void ControlPanel::onPlay(CCObject*) {
    auto* rs = ReplaySystem::get();
    if (!rs->isPlaying())
        rs->startPlayback();
}

void ControlPanel::onStop(CCObject*) {
    auto* rs = ReplaySystem::get();
    if (rs->isRecording()) rs->stopRecording();
    if (rs->isPlaying())   rs->stopPlayback();
}

void ControlPanel::onSave(CCObject*) {
    // Open a native file-save dialog via Geode utils.
    file::FilePickOptions opts;
    opts.filters = {{ "zzBot Macro (*.zzb)", { "*"*.zzb" } }};

    file::pickFile(file::PickMode::SaveFile, opts, [](ghc::filesystem::path path) {
        ReplaySystem::get()->saveToFile(path.string());
    });
}

void ControlPanel::onLoad(CCObject*) {
    file::FilePickOptions opts;
    opts.filters = {{ "zzBot Macro (*.zzb)", { "*"*.zzb" } }};

    file::pickFile(file::PickMode::OpenFile, opts, [](ghc::filesystem::path path) {
        ReplaySystem::get()->loadFromFile(path.string());
    });
}

void ControlPanel::onEnterStepper(CCObject*) {
    FrameStepper::get()->enter();
}

void ControlPanel::onExitStepper(CCObject*) {
    FrameStepper::get()->exit();
}

void ControlPanel::onToggleHitbox(CCObject*) {
    auto* hv = HitboxViewer::get();
    hv->setShowHitbox(!hv->showHitbox());
    Mod::get()->setSavedValue("show-hitboxes", hv->showHitbox());
}

void ControlPanel::onToggleTrail(CCObject*) {
    auto* hv = HitboxViewer::get();
    hv->setShowTrail(!hv->showTrail());
    Mod::get()->setSavedValue("show-hitbox-trail", hv->showTrail());
}

void ControlPanel::onToggleTrajectory(CCObject*) {
    auto* ts = TrajectorySystem::get();
    ts->setEnabled(!ts->isEnabled());
    Mod::get()->setSavedValue("show-trajectory", ts->isEnabled());
}

// ─────────────────────────────────────────────────────────────────────────────
// Status refresh timer
// ─────────────────────────────────────────────────────────────────────────────

void ControlPanel::refreshStatus(float) {
    if (!m_statusLabel) return;

    auto* rs = ReplaySystem::get();
    std::string stateStr;
    switch (rs->state()) {
        case BotState::Idle:      stateStr = "Idle";      break;
        case BotState::Recording: stateStr = "Recording"; break;
        case BotState::Playing:   stateStr = "Playing";   break;
    }

    bool stepping = FrameStepper::get()->isActive();
    int  tick     = 0;
    if (auto* pl = PlayLayer::get()) tick = pl->m_nCurrentTick;

    m_statusLabel->setString(fmt::format(
        "State: {}  |  Frames: {}  |  Tick: {}  |  Stepper: {}  |  Speed: {:.2f}x",
        stateStr,
        rs->currentMacro().frames.size(),
        tick,
        stepping ? "ON" : "off",
        SpeedControl::get()->speed()
    ).c_str());

    if (m_speedLabel)
        m_speedLabel->setString(
            fmt::format("Current: {:.2f}x", SpeedControl::get()->speed()).c_str());

    if (m_fpsLabel)
        m_fpsLabel->setString(
            fmt::format("FPS:{:.0f} TPS:{:.0f}",
                FPSUnlocker::get()->targetFPS(),
                TPSModifier::get()->tps()).c_str());
}
