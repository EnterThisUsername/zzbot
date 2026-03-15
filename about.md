# zzBot

zzBot is a botting mod for Geometry Dash built to let you record and replay with frame-perfect accuracy. Whether you're making a showcase, testing a tight section, or just wanna do a swag route, zzBot lets you.

---

## Replay System

This is the main feature. When you hit Record, zzBot starts recording your inputs and logging them to memory. It only saves the moments when something actually changes — a button press or release — along with the exact physics tick it happened on. That means the files stay small and it doesn't take up all your resources

When you hit Play, those inputs get injected back into the game on the exact same ticks they were originally recorded on. As long as the level and physics settings match, the run will reproduce identically every single time! You can save your macros as `.zzb` files (a simple JSON format you can open in any text editor, but '.zzb' for looks) and load them back whenever you want!

This is the same basic approach used by bots like xdBot, implemented natively as a Geode mod so it works without any other external tools.

---

## Speed Control

Sometimes 1x speed isn't solid for recording very percise inputs — like frame perfects. zzBot lets you slow the entire game down — not just the visuals; instead the actual physics simulation — so you have more time to react. You can go as low as 1% speed, which turns each second of gameplay into around 100 real-world seconds.

Presets are available directly in the control panel: 0.01x, 0.1x, 0.25x, 0.5x, 1.0x, and 2.0x. The macro records tick indices rather than timestamps, so a run recorded at 0.5x speed will still replay correctly at 1.0x (unlike some others).

---

## Frame Stepper

When speed control isn't precise enough, the frame stepper lets you move through the game one physics tick at a time.

- Press **F** to advance forward by one tick
- Press **G** to exit the stepper and resume at 50% speed

This is mostly useful for testing and checking — you can isolate the exact tick where something goes wrong and fix your macro around it!

---

## FPS Unlocker

Geometry Dash caps its render loop at 60 FPS by default. zzBot intercepts the internal frame timer and replaces it with a target of your choice. Presets go up to 480 FPS, and there's an unlimited option that removes the cap entirely and lets your system run as fast as it can without anything stopping it.

Physics and rendering are handled separately, so changing your FPS doesn't break the replay system at all.

---

## TPS Modifier

The game's physics simulation normally runs at 240 ticks per second. zzBot lets you change this. Raising the TPS means physics updates happen more often, which can give you sub-frame precision in your recordings. Lowering it does the opposite, which is worse.

This is an advanced feature — changing TPS between recording and playback sessions will break the replay, so make sure they match. This is a warning!

---

## Trajectory Prediction

When this is turned on, zzBot runs a quick physics simulation starting from your current position and draws the predicted path as a line of fading green dots in front of you. It accounts for your current game mode (cube, ship, ball, UFO, wave, robot, spider, swing), gravity direction, and whether you're holding the jump button or not.

It's not perfectly accurate for every situation — things like speed portals mid-trajectory won't be accounted for yet — but it's close enough to be genuinely useful for learning tight sections or planning inputs in slow motion.

---

## Hitbox Visualiser

Draws your player's actual collision square/rectangle on screen so you can see exactly what the game is using for hit detection, including the danger hitbox and the solid hitbox. There's also a trail mode that records your hitbox position every tick and renders the history as a fading yellow box/rectangle trail behind you. This makes it easy to see how close you came to clipping an obstacle without actually dying, mainly for replay purposes.

Trail length is configurable in the Mod menu.

---

## Click Sounds

When enabled, zzBot plays a sound at the exact tick a button is pressed, and a separate sound at the exact tick it's released. Both sounds are fully customizable — you can swap them out for whatever you want. This is useful for making replays sound satisfying or for syncing audio to your showcase.

---

## Control Panel

Everything is accessible from a single panel that opens from the pause menu. Tap the **zzBot** button while paused to open it. From there you can start and stop recording, manage playback, open and save macro files, toggle visuals, adjust speed, change FPS, and enter or exit the frame stepper. Settings that persist between sessions (FPS target, TPS, hitbox toggles, etc.) are saved through Geode's built-in settings system.
