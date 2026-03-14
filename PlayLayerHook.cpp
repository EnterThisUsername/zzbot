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

using namespace geode::prelude;

// ─────────────────────────────────────────────────────────────────────────────
// Globals / per-level state
// ─────────────────────────────────────────────────────────────────────────────

namespace {
    VisualizerNode* g_vizNode = nullptr;

    // Physics accumulator for custom TPS.
    // GD normally calls physics at a fixed interval; we intercept it.
    double g_physAccum   = 0.0;
    int    g_levelTick   = 0;       // our own tick counter (independent of GD's)
}

// ─────────────────────────────────────────────────────────────────────────────
// PlayLayer hook
// ─────────────────────────────────────────────────────────────────────────────

class $modify(BotPlayLayer, PlayLayer) {

    // ── Level entry ──────────────────────────────────────────────────────────

    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
        if (!PlayLayer::init(level, useReplay, dontCreateObjects)) return false;

        // Reset per-level state.
        g_physAccum = 0.0;
        g_levelTick = 0;
        HitboxViewer::get()->clearTrail();

        // Attach the visualizer overlay node.
        g_vizNode = VisualizerNode::create();
        if (g_vizNode) this->addChild(g_vizNode, 1000);

        // Apply stored settings.
        FPSUnlocker::get()->apply();
        SpeedControl::get()->setSpeed(
            Mod::get()->getSavedValue<float>("speed-multiplier", 1.0f));

        return true;
    }

    // ── Level exit ───────────────────────────────────────────────────────────

    void onQuit() {
        // Stop any active recording / playback gracefully.
        auto* rs = ReplaySystem::get();
        if (rs->isRecording()) rs->stopRecording();
        if (rs->isPlaying())   rs->stopPlayback();

        // Deactivate frame stepper.
        if (FrameStepper::get()->isActive())
            FrameStepper::get()->exit();

        // Restore normal scheduler speed.
        SpeedControl::get()->reset();

        g_vizNode   = nullptr;
        g_levelTick = 0;

        PlayLayer::onQuit();
    }

    // ── Physics update ───────────────────────────────────────────────────────
    //
    // GJBaseGameLayer::update() is the main physics ticker.
    // We intercept it here to:
    //   1. Honour the custom TPS (accumulator pattern).
    //   2. Let FrameStepper gate individual ticks.
    //   3. Inject replay inputs at the exact tick they were recorded.
    //   4. Record inputs (handled in the input hook below).
    //   5. Notify VisualizerNode for trail updates.

    void update(float dt) {
        auto* tps      = TPSModifier::get();
        auto* stepper  = FrameStepper::get();
        auto* rs       = ReplaySystem::get();

        // ── Frame Stepper gate ───────────────────────────────────────────────
        // If the stepper says "don't tick", skip the physics update entirely.
        if (stepper->isActive()) {
            if (!stepper->onPhysicsTick(g_levelTick)) {
                // Still allow render / UI to run.
                PlayLayer::update(0.0f);
                return;
            }
        }

        // ── Custom TPS accumulator ────────────────────────────────────────────
        const double step = static_cast<double>(tps->physicsStep());
        g_physAccum += static_cast<double>(dt);

        // Run as many physics ticks as the accumulator allows.
        while (g_physAccum >= step) {
            g_physAccum -= step;

            // Inject playback inputs for this tick.
            if (rs->isPlaying())
                rs->tickPlayback(g_levelTick);

            // Tick GD physics with exactly one fixed step.
            PlayLayer::update(static_cast<float>(step));

            // Update visualizer trail.
            if (g_vizNode) g_vizNode->onPhysicsTick();

            ++g_levelTick;

            // After a single-step advance, re-freeze the scheduler.
            if (stepper->isActive()) {
                if (auto* s = CCDirector::get()->getScheduler())
                    s->setTimeScale(0.0f);
                break;
            }
        }
    }

    // ── Level reset ──────────────────────────────────────────────────────────

    void resetLevel() {
        g_physAccum = 0.0;
        g_levelTick = 0;
        HitboxViewer::get()->clearTrail();

        if (ReplaySystem::get()->isPlaying()) {
            // Restart the playback index when the level resets.
            ReplaySystem::get()->startPlayback();
        }

        PlayLayer::resetLevel();
    }
};

// ─────────────────────────────────────────────────────────────────────────────
// Input hook — intercept handleButton to record inputs
// ─────────────────────────────────────────────────────────────────────────────

class $modify(BotInputPlayLayer, PlayLayer) {

    void handleButton(bool down, int button, bool isPlayer1) {
        // During playback, block real inputs so the macro is authoritative.
        if (ReplaySystem::get()->isPlaying()) return;

        // Record the input transition.
        if (ReplaySystem::get()->isRecording())
            ReplaySystem::get()->recordInput(g_levelTick, button, down, !isPlayer1);

        PlayLayer::handleButton(down, button, isPlayer1);
    }
};

// ─────────────────────────────────────────────────────────────────────────────
// Keyboard hook — X / C / V keys for frame stepper
// ─────────────────────────────────────────────────────────────────────────────

#include <Geode/modify/CCKeyboardDispatcher.hpp>

class $modify(BotKeyboard, CCKeyboardDispatcher) {

    bool dispatchKeyboardMSG(enumKeyCodes key, bool isKeyDown, bool isKeyRepeat) {
        // Only handle key-down, no repeat for stepper keys.
        if (isKeyDown && !isKeyRepeat) {
            auto* stepper = FrameStepper::get();

            switch (key) {
            case KEY_X:
                if (stepper->isActive()) {
                    stepper->stepForward();
                    return true;
                }
                break;

            case KEY_C:
                if (stepper->isActive()) {
                    stepper->stepBack();
                    return true;
                }
                break;

            case KEY_V:
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
