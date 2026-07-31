#pragma once

#include <cstdint>
#include <Config/RangeConstrainedVariableParams.h>

namespace aimbot_params
{

// FOV pixel radius around the crosshair on a 1920px-wide reference screen.
// Aimbot::calculateNdcRadius scales this to the actual resolution so the
// number reflects roughly the same visible circle across monitors.
constexpr auto kFovRadius = RangeConstrainedVariableParams<std::uint8_t>{.min = 30, .max = 250, .def = 150};
constexpr float kReferenceScreenWidth = 1920.0f;

// Z offset above absOrigin (feet) for each aim point. Mirrors the order in
// AimbotAimPoint enum: Head/Neck/Chest/Stomach/Pelvis.
constexpr float kAimPointZOffset[5]{64.0f, 56.0f, 48.0f, 36.0f, 20.0f};

// Forward projection time for velocity prediction. Compensates for the
// "we aim where the enemy WAS one frame ago" lag. CS2 server-side lag
// compensation already handles network latency for the shot itself, so we
// only need ~1 frame of prediction here.
constexpr float kVelocityPredictionTimeSeconds{0.015f};

// ===== Humanization / concealment constants =====
// Each value injects small amounts of controlled randomness to defeat VACnet /
// XGuardian trajectory-classification ML while staying within plausible human aim.

// Max angular jitter added to the computed aim angle (± degrees).
// Human aim always has ~0.1-0.3° of natural micro-tremor — we keep it on the
// low end so snap-mode accuracy isn't degraded while still defeating "exact
// bone-to-eye" XGuardian signatures.
constexpr float kAimJitterMaxDegrees{0.08f};

// Smooth factor range for per-engagement randomization. Re-rolled whenever
// the target changes so the convergence rate is different for each enemy.
constexpr float kSmoothFactorMin{0.12f};
constexpr float kSmoothFactorMax{0.48f};

// Per-frame smooth factor drift (random walk within [min, max]).
constexpr float kSmoothFactorDriftMax{0.015f};

// Max per-frame angle delta (degrees) when humanization is on.
// VACnet flags >30° single-frame flicks as impossible for humans.
constexpr float kMaxAngleDeltaPerFrame{22.0f};

// Probability of skipping an aim update this frame, creating natural gaps.
// Kept low — every skip is one frame where a moving target drifts away from
// the locked angle.
constexpr float kFrameSkipProbability{0.03f};

// Velocity prediction time randomization range (seconds).
// Reduced from 25-75ms because CS2 hitscan + server lag compensation means we
// shouldn't be aiming "ahead" much — only enough to cover one render frame.
constexpr float kVelocityPredictionTimeMin{0.008f};
constexpr float kVelocityPredictionTimeMax{0.025f};

// Max spatial offset added to the aim target position (game units).
// This noise is angularly amplified by 1/distance — large at close range, so
// we keep it very small (≤0.1° at all practical distances).
constexpr float kBoneOffsetMaxUnits{0.20f};

// ===== Woodworth two-component motor model =====
// Phase durations in REAL SECONDS — framerate-independent so the engagement
// timeline is identical at any framerate. Randomized at each engagement so
// XGuardian's GRU can't fingerprint constant phase durations.
// Reaction time is the "continuing engagement" range (~30-80ms) instead of
// the cold-start visual reaction (~150ms). Players holding an aim hotkey
// have already visually acquired the target, so the latency should match
// the much shorter motor-only response.
constexpr float kReactionTimeMinSec{0.030f};
constexpr float kReactionTimeMaxSec{0.080f};
constexpr float kBallisticTimeMinSec{0.060f}; // ballistic reach
constexpr float kBallisticTimeMaxSec{0.120f};
constexpr float kCorrectiveTimeMinSec{0.040f}; // feedback closure
constexpr float kCorrectiveTimeMaxSec{0.080f};

// Overshoot fraction of remaining angular distance at end of ballistic.
// Human flicks typically overshoot 10-25% then correct back.
constexpr float kOvershootFractionMin{0.10f};
constexpr float kOvershootFractionMax{0.25f};

// Corrective and Hold phase factors are quoted at a reference 60fps and
// scaled linearly by (dt * 60) at runtime so closure-rate stays consistent
// regardless of game framerate.
// Hold factor was 0.18 originally → for a 250 u/s strafing target this gave
// ~5° tracking lag (steady-state lag = v·dt·(1-f)/f). Bumped to 0.55 so the
// lag drops to ~0.5° while still being recognizable as "human tracking" with
// minor corrections rather than rigid lock-on.
constexpr float kCorrectiveLerpFactor60{0.55f};
constexpr float kHoldLerpFactor60{0.55f};
constexpr float kLerpFactorMaxCap{0.95f};
constexpr float kReferenceFps{60.0f};

// Tremor — multi-harmonic sinusoid mimics 1/f hand-tremor noise (~8Hz).
// Phase advances in radians-per-second so the visible vibration frequency
// matches real human hand tremor independent of framerate.
constexpr float kTremorPhaseSpeedRadPerSec{33.0f}; // ~5.2 Hz fundamental
constexpr float kTremorAmpMoving{0.06f};   // ~0.06° during move (was 0.18)
constexpr float kTremorAmpHold{0.025f};    // ~0.025° while held on target

// Kill cooldown: max successive kills before aimbot pauses. DM spawns
// produce rapid kill chains that VACnet flags. Range 3-15 kills, default 5.
constexpr auto kKillCooldown = RangeConstrainedVariableParams<std::uint8_t>{.min = 2, .max = 20, .def = 5};

// Miss chance: per-engagement probability of intentionally missing a shot.
// Scrambles the headshot-ratio statistic. Range 0-25%.
constexpr auto kMissChance = RangeConstrainedVariableParams<std::uint8_t>{.min = 0, .max = 25, .def = 8};
// Lock-break: after N consecutive frames locked, disengage for 2-6 frames.
// Lower = more human-like but more frequent disconnects from target.
// Range 15-120 frames (~0.25-2s at 60fps).
constexpr auto kLockBreakInterval = RangeConstrainedVariableParams<std::uint8_t>{.min = 15, .max = 120, .def = 40};

// Sanity bounds for the per-frame delta time. Outside this range we use a
// 60fps fallback — protects against zeroed/paused curtime and from sudden
// hitches that would otherwise advance the phase machine by huge jumps.
constexpr float kMinValidDtSec{0.001f};
constexpr float kMaxValidDtSec{0.250f};
constexpr float kFallbackDtSec{1.0f / 60.0f};

}
