#pragma once

#include <cstdint>

#include <Config/ConfigVariable.h>
#include "AimbotParams.h"

enum class AimbotAimPoint : std::uint8_t {
    Head = 0,
    Neck = 1,
    Chest = 2,
    Stomach = 3,
    Pelvis = 4,
};

// Hot-key gating the aimbot. When set to a real key, aim assist only runs
// while that key is held down — this dramatically cuts VAC suspicion because
// the angle changes look like deliberate user engagement, not 24/7 auto-snap.
enum class AimbotKey : std::uint8_t {
    Always = 0, RightMouse = 1, Mouse5 = 2, Shift = 3, CapsLock = 4,
};

enum class AimbotMovementPenalty : std::uint8_t {
    Off = 0, Rifles = 1, All = 2,
};

enum class AimbotLockMode : std::uint8_t {
    Crosshair = 0, // only enemies near visible crosshair (FOV-limited, safest)
    Closest = 1,   // closest enemy by angle, even off-screen (360°)
};

namespace aimbot_vars
{
    CONFIG_VARIABLE(Enabled, bool, false);
    CONFIG_VARIABLE(Smooth, bool, false);
    CONFIG_VARIABLE_RANGE(FovRadius, aimbot_params::kFovRadius);
    CONFIG_VARIABLE(AimPoint, AimbotAimPoint, AimbotAimPoint::Head);
    CONFIG_VARIABLE(PredictVelocity, bool, true);
    CONFIG_VARIABLE(Humanization, bool, true);
    // Enabling Aimbot should be sufficient for a fresh configuration. Users
    // who want hold-to-aim can opt into a key in the UI.
    CONFIG_VARIABLE(HotKey, AimbotKey, AimbotKey::Always);
    // Silent aim: modify server-side viewangles without moving the crosshair.
    // Keep visible aim as the default so the top-level switch has immediate,
    // observable feedback; silent aim remains available as an opt-in mode.
    CONFIG_VARIABLE(SilentAim, bool, false);
    // Chain-kill cooldown. After N successive kills, pauses for M seconds.
    CONFIG_VARIABLE_RANGE(KillCooldown, aimbot_params::kKillCooldown);
    // Random miss probability per engagement.
    CONFIG_VARIABLE_RANGE(MissChance, aimbot_params::kMissChance);
    // Movement penalty mode:
    //   Off (0) = always aim regardless of movement
    //   Rifles (1) = only disengage when holding rifle/sniper (SMG/pistol still work)
    //   All (2) = disengage while running with any weapon
    CONFIG_VARIABLE(MovementPenalty, AimbotMovementPenalty, AimbotMovementPenalty::Rifles);
    // Lock-breaking: after N successive frames locked on the same target,
    // briefly disengage for a randomized gap. Humans constantly micro-correct
    // their aim; a perfect lock with zero breaks is a dead giveaway even when
    // the angles themselves aren't visible.
    CONFIG_VARIABLE_RANGE(LockBreakInterval, aimbot_params::kLockBreakInterval);
    // Lock mode: Crosshair = FOV-limited NDC check; Closest = 360° angular
    // nearest enemy regardless of crosshair position.
    CONFIG_VARIABLE(LockMode, AimbotLockMode, AimbotLockMode::Crosshair);
    // Glow the locked target briefly so you know who the aimbot is tracking.
    // Uses the existing outline glow system with a distinct color.
    CONFIG_VARIABLE(LockIndicator, bool, true);
}
