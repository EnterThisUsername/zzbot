# zzBot — Geode Mod for Geometry Dash 2.2081

A comprehensive TAS and replay mod: frame-accurate macro recording/playback,
speed control, FPS/TPS override, trajectory prediction, hitbox visualisation,
and a frame-by-frame stepper.

Repository: https://github.com/EnterThisUsername/zzbot

---

## Requirements

| Dependency | Version |
|---|---|
| Geometry Dash | 2.2081 (Steam / Mac) |
| Geode SDK | ≥ 5.3.0 |
| CMake | ≥ 3.21 |
| Visual Studio | 2022 (Windows) — must include the **"Desktop development with C++"** workload |
| Ninja | Ships with Visual Studio; used as the CMake generator |

---

## Build

> **Windows:** Always run these commands inside a **"Developer Command Prompt for VS 2022"**
> (or a terminal where `vcvarsall.bat` has been sourced). This puts `ninja.exe`
> and the MSVC compiler on `PATH` — which is exactly what was missing when you
> saw the `nmake not found` error.

```bat
REM 1. Install Geode CLI and SDK  (https://geode-sdk.org/install)
geode sdk install

REM 2. Clone
git clone https://github.com/EnterThisUsername/zzbot
cd zzbot

REM 3. Configure  (CMakePresets.json selects Ninja automatically)
cmake --preset win-release

REM 4. Build
cmake --build --preset win-release

REM 5. Install into Geode's mods folder
cmake --build build --target install
```

If you prefer not to use presets:

```bat
cmake -B build -G "Ninja" -DCMAKE_BUILD_TYPE=RelWithDebInfo
cmake --build build
```

Never use `cmake -B build` on Windows without `-G` — CMake will try to detect
a Make tool and fall back to NMake, which is not installed in a standard VS
setup.

---

## Features

### Replay System
The core of the mod. Stores only input *transitions* (press / release) tagged
with the physics tick they occurred on, not every frame.

| Action | How |
|---|---|
| Start Recording | Pause menu → zzBot → **Record** |
| Stop Recording | **Stop** button |
| Start Playback | **Play** button |
| Stop Playback | **Stop** button |
| Save macro | **Save** → file picker (`.zzb` JSON) |
| Load macro | **Load** → file picker |

Macro file format (`.zzb`, JSON):
```json
{
  "name": "my_run",
  "frames": [
    { "tick": 0,   "button": 1, "press": true,  "p2": false },
    { "tick": 42,  "button": 1, "press": false, "p2": false }
  ]
}
```

### Speed Control
Scale the Cocos2d scheduler time-scale uniformly.

| Value | Effect |
|---|---|
| `1.0` | Normal speed |
| `0.5` | Half speed — good for manual macro recording |
| `0.25` | Quarter speed |
| `0.01` | 1% — essentially frame-by-frame without the stepper |
| `2.0` | Double speed |

Presets available in the control panel. Persistent across sessions.

### FPS Unlocker
Overrides `CCDirector::setAnimationInterval` to prevent GD from capping the
render loop at 60 FPS. Presets: 60, 120, 240, 480, Unlimited.

Setting **Unlimited** uses an interval of `1/10000s`, effectively removing the
cap so the system runs as fast as the GPU/CPU allows.

### Physics TPS Modifier
Changes how often the physics accumulator fires per second (default 240 Hz).

- Raising TPS increases update frequency → more precise collision and gravity.
- Lowering TPS gives coarser physics → useful for testing at reduced rates.

The physics step `dt` passed to `PlayLayer::update` is always `1 / TPS` so
the simulation remains deterministic at any chosen rate.

### Frame Stepper

| Key | Action |
|---|---|
| `X` | Advance one physics tick forward |
| `C` | Retreat one tick (resets + replays to tick−1) |
| `V` | Exit stepper, resume at 50% speed |

Enter via the control panel **Enter Stepper** button. The scheduler
time-scale is set to `0` between steps so nothing advances without your input.

### Trajectory Prediction
Runs a forward physics simulation starting from the player's current state and
draws the predicted path as fading green dots. Accounts for gravity direction,
game mode (cube / ship / ball / UFO / wave / robot / spider), and held inputs.

Configurable lookahead (30–960 ticks) in Geode settings.

### Hitbox Visualiser
- **Hitboxes** — draws the player's outer collision rect in cyan each frame.
- **HB Trail** — records the hitbox rect every physics tick and renders a
  yellow fading trail showing where the player has been.

---

## Settings (mod.json / Geode settings UI)

| Key | Default | Description |
|---|---|---|
| `target-fps` | 240 | Render frame cap (0 = unlimited) |
| `target-tps` | 240 | Physics update rate |
| `speed-multiplier` | 1.0 | Game speed scale |
| `show-hitboxes` | false | Live hitbox overlay |
| `show-hitbox-trail` | false | Hitbox trail |
| `hitbox-trail-length` | 240 | Trail history in frames |
| `show-trajectory` | false | Trajectory prediction dots |
| `trajectory-steps` | 240 | Lookahead tick count |

---

## Architecture

```
src/
├── main.cpp                  — $on_mod(Loaded) entry, settings init
├── ReplaySystem.{hpp,cpp}    — Frame-accurate recording & playback engine
├── SpeedControl.{hpp,cpp}    — CCScheduler time-scale wrapper
├── FPSUnlocker.{hpp,cpp}     — setAnimationInterval override
├── TPSModifier.{hpp,cpp}     — Physics tick rate store
├── FrameStepper.{hpp,cpp}    — Tick-by-tick stepping controller
├── TrajectorySystem.{hpp,cpp}— Forward physics simulation
├── HitboxViewer.{hpp,cpp}    — Trail data management
├── VisualizerNode.{hpp,cpp}  — CCNode overlay: draws all visuals
└── hooks/
    ├── PlayLayerHook.cpp     — Physics tick, input recording, level lifecycle
    ├── PauseLayerHook.cpp    — Injects pause menu button
    └── CCDirectorHook.cpp    — Prevents GD from resetting animation interval
```

---

## Determinism Notes

For a macro to replay identically:
- The level must be the same revision and the same song offset.
- Speed must be `1.0` during playback (or the same speed used during recording).
- TPS must match between recording and playback sessions.

The `.zzb` file stores only logical tick indices, not wall-clock times, so the
replay is independent of FPS and system performance.

---

## License

MIT — see `LICENSE`.