#include "FrameStepper.hpp"
#include "SpeedControl.hpp"
#include "ReplaySystem.hpp"

FrameStepper* FrameStepper::get() {
    static FrameStepper s_instance;
    return &s_instance;
}

// ─────────────────────────────────────────────────────────────────────────────
void FrameStepper::enter() {
    m_active      = true;
    m_doStep      = false;

    // Record the current physics tick from PlayLayer if available.
    if (auto* pl = PlayLayer::get())
        m_tick = pl->m_nCurrentTick;

    // Freeze the scheduler so the game logic stops advancing on its own.
    if (auto* s = CCDirector::get()->getScheduler())
        s->setTimeScale(0.0f);

    log::info("[BotToolkit] Frame stepper ACTIVE at tick {}.", m_tick);
}

void FrameStepper::exit() {
    m_active  = false;
    m_doStep  = false;

    // Resume at 50% speed as a safe, controllable default.
    SpeedControl::get()->setSpeed(0.5f);

    log::info("[BotToolkit] Frame stepper exited. Resuming at 50% speed.");
}

void FrameStepper::stepForward() {
    if (!m_active) return;

    // Signal the physics hook to allow exactly one tick through.
    m_doStep = true;

    // Un-freeze for one scheduler cycle.  The hook re-freezes after the tick.
    if (auto* s = CCDirector::get()->getScheduler())
        s->setTimeScale(1.0f);
}

void FrameStepper::stepBack() {
    if (!m_active || m_tick <= 0) return;

    int targetTick = m_tick - 1;
    log::info("[BotToolkit] Stepping back to tick {}.", targetTick);

    // Stop any ongoing playback.
    auto* rs = ReplaySystem::get();
    rs->stopPlayback();

    // Reset the level.
    if (auto* pl = PlayLayer::get()) {
        pl->resetLevel();

        // Start playback of the stored macro up to targetTick.
        if (!rs->currentMacro().frames.empty()) {
            rs->startPlayback();
            // The physics hook will stop injecting inputs once tick == targetTick
            // and re-enters step mode via onPhysicsTick().
        }
    }

    m_tick   = targetTick;
    // Briefly run at full speed; the hook pauses again when targetTick is reached.
    if (auto* s = CCDirector::get()->getScheduler())
        s->setTimeScale(1.0f);
}

// ─────────────────────────────────────────────────────────────────────────────
bool FrameStepper::onPhysicsTick(int tick) {
    if (!m_active) return true; // normal execution

    // Stepping-back replay: run until we reach the target tick, then freeze.
    if (tick == m_tick && !m_doStep) {
        // We have reached our target — freeze again.
        if (auto* s = CCDirector::get()->getScheduler())
            s->setTimeScale(0.0f);
        return false; // suppress this tick
    }

    if (m_doStep) {
        m_doStep         = false;
        m_stepInProgress = true;
        m_tick           = tick;
        // Allow this tick to proceed.
        return true;
    }

    // Between ticks during a normal freeze: suppress.
    return false;
}
