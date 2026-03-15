#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>

#include "../ReplaySystem.hpp"
#include "../SpeedControl.hpp"
#include "../FPSUnlocker.hpp"
#include "../TPSModifier.hpp"
#include "../FrameStepper.hpp"
#include "../HitboxViewer.hpp"
#include "../TrajectorySystem.hpp"
#include "../VisualizerNode.hpp"
#include "../ClickSound.hpp"

using namespace geode::prelude;

// ─────────────────────────────────────────────────────────────────────────────
// Per-level state
// ─────────────────────────────────────────────────────────────────────────────

namespace {
    VisualizerNode* g_vizNode   = nullptr;
    double          g_physAccum = 0.0;
    int             g_levelTick = 0;
}

// ─────────────────────────────────────────────────────────────────────────────
// PlayLayer hook
// ─────────────────────────────────────────────────────────────────────────────

class $modify(BotPlayLayer, PlayLayer) {

    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
        if (!PlayLayer::init(level, useReplay, dontCreateObjects)) return false;

        g_physAccum = 0.0;
        g_levelTick = 0;
        HitboxViewer::get()->clearTrail();

        g_vizNode = VisualizerNode::create();
        if (g_vizNode) this->addChild(g_vizNode, 1000);

        FPSUnlocker::get()->apply();
        SpeedControl::get()->setSpeed(
            Mod::get()->getSavedValue<float>("speed-multiplier", 1.0f));

        return true;
    }

    void onQuit() {
        auto* rs = ReplaySystem::get();
        if (rs->isRecording()) rs->stopRecording();
        if (rs->isPlaying())   rs->stopPlayback();

        if (FrameStepper::get()->isActive())
            FrameStepper::get()->exit();

        SpeedControl::get()->reset();

        g_vizNode   = nullptr;
        g_levelTick = 0;

        PlayLayer::onQuit();
    }

    void update(float dt) {
        auto* tps     = TPSModifier::get();
        auto* stepper = FrameStepper::get();
        auto* rs      = ReplaySystem::get();

        if (stepper->isActive()) {
            if (!stepper->onPhysicsTick(g_levelTick)) {
                PlayLayer::update(0.0f);
                return;
            }
        }

        const double step = static_cast<double>(tps->physicsStep());
        g_physAccum += static_cast<double>(dt);

        while (g_physAccum >= step) {
            g_physAccum -= step;

            if (rs->isPlaying())
                rs->tickPlayback(g_levelTick);

            PlayLayer::update(static_cast<float>(step));

            if (g_vizNode) g_vizNode->onPhysicsTick();

            ++g_levelTick;

            if (stepper->isActive()) {
                if (auto* s = CCDirector::get()->getScheduler())
                    s->setTimeScale(0.0f);
                break;
            }
        }
    }

    void resetLevel() {
        g_physAccum = 0.0;
        g_levelTick = 0;
        HitboxViewer::get()->clearTrail();

        if (ReplaySystem::get()->isPlaying())
            ReplaySystem::get()->startPlayback();

        PlayLayer::resetLevel();
    }
};

// ─────────────────────────────────────────────────────────────────────────────
// Input hook — record inputs; block real inputs during playback;
//              trigger click sounds at the exact injected tick
// ─────────────────────────────────────────────────────────────────────────────

class $modify(BotInputPlayLayer, PlayLayer) {

    void handleButton(bool down, int button, bool isPlayer1) {
        auto* rs = ReplaySystem::get();

        if (rs->isPlaying()) {
            // The input came from the replay engine — play the click sound.
            if (down)
                ClickSound::get()->playPress();
            else
                ClickSound::get()->playRelease();

            // Still call the original so the input actually takes effect.
            PlayLayer::handleButton(down, button, isPlayer1);
            return;
        }

        // Block real user input during playback (already handled above by
        // early return). During recording, log and pass through normally.
        if (rs->isRecording())
            rs->recordInput(g_levelTick, button, down, !isPlayer1);

        PlayLayer::handleButton(down, button, isPlayer1);
    }
};

// ─────────────────────────────────────────────────────────────────────────────
// Keyboard hook — F = step forward, G = exit stepper
// ─────────────────────────────────────────────────────────────────────────────

#include <Geode/modify/CCKeyboardDispatcher.hpp>

class $modify(BotKeyboard, CCKeyboardDispatcher) {

    bool dispatchKeyboardMSG(enumKeyCodes key, bool isKeyDown, bool isKeyRepeat) {
        if (isKeyDown && !isKeyRepeat) {
            auto* stepper = FrameStepper::get();

            switch (key) {
            case KEY_F:
                if (stepper->isActive()) {
                    stepper->stepForward();
                    return true;
                }
                break;

            case KEY_G:
                if (stepper->isActive()) {
                    stepper->exit();
                    return true;
                }
                break;

            default:
                break;
            }
        }

        return CCKeyboardDispatcher::dispatchKeyboardMSG(key, isKeyDown, isKeyRepeat);
    }
};
