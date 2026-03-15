#include "FrameStepper.hpp"
#include "SpeedControl.hpp"

FrameStepper* FrameStepper::get() {
    static FrameStepper s_instance;
    return &s_instance;
}

void FrameStepper::enter() {
    m_active  = true;
    m_doStep  = false;

    if (auto* pl = PlayLayer::get())
        m_tick = pl->m_nCurrentTick;

    // Freeze the scheduler so the game stops advancing on its own.
    if (auto* s = CCDirector::get()->getScheduler())
        s->setTimeScale(0.0f);

    log::info("[zzBot] Frame stepper ACTIVE at tick {}.", m_tick);
}

void FrameStepper::exit() {
    m_active = false;
    m_doStep = false;

    // Resume at 50% speed as a safe default.
    SpeedControl::get()->setSpeed(0.5f);

    log::info("[zzBot] Frame stepper exited. Resuming at 50% speed.");
}

void FrameStepper::stepForward() {
    if (!m_active) return;

    m_doStep = true;

    // Un-freeze for one scheduler cycle; the hook re-freezes after the tick.
    if (auto* s = CCDirector::get()->getScheduler())
        s->setTimeScale(1.0f);
}

bool FrameStepper::onPhysicsTick(int tick) {
    if (!m_active) return true;

    if (m_doStep) {
        m_doStep = false;
        m_tick   = tick;
        return true;   // allow this single tick through
    }

    // Frozen between steps — suppress the tick and re-lock the scheduler.
    if (auto* s = CCDirector::get()->getScheduler())
        s->setTimeScale(0.0f);

    return false;
}
