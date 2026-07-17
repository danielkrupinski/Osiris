#pragma once

#include <CS2/Classes/Vector.h>
#include <Utils/ColorUtils.h>
#include <Features/Visuals/GrenadePrediction/GrenadeKind.h>

namespace grenade_prediction_params
{
    // --- Simulation physics constants ---
    // Simulation assumptions and regression values. They are not a claim of
    // current client/server binary parity.

    constexpr float kSimDt = 1.0f / 64.0f;
    constexpr float kSvGravity = 800.0f;
    // Regression stop-speed value: speed² < 400.0 (speed < 20 u/s).
    constexpr float kStopSpeedSq = 400.0f;
    constexpr float kMolotovSlope = 0.8660254f; // cos(30)
    // Must exceed 18 seconds at 64 Hz (the smoke safety cap is a strict > check).
    // Trajectory point storage remains bounded independently by pointsCount.
    constexpr int   kMaxTicks = 1154;
    constexpr int   kTicksPerPoint = 2;
    // Regression push-off epsilon used to keep contacts outside surfaces.
    constexpr float kClipPushOff = 0.03125f;

    // Displacement threshold for velocity-stop grenades (smoke/decoy).
    // Regression displacement threshold (FLT_EPSILON squared).
    constexpr float kStopDisplacementSq = 1.42e-14f; // FLT_EPSILON squared

    constexpr int   kMaxBounces = 20;
    constexpr int   kMaxCollisionPasses = 4;
    constexpr float kRemainingFractionEpsilon = 0.001f;

    // --- Per-grenade physics ---
    struct GrenadePhysics {
        float gravityScale;
        float elasticity;
    };

    // Regression physics values shared by all supported grenade kinds.
    constexpr GrenadePhysics getGrenadePhysics(cs2::GrenadeKind /*kind*/) {
        return {0.40f, 0.45f};
    }

    // --- Throw parameters ---
    // Assumed base throw velocity for all supported grenade kinds.
    constexpr float kBaseThrowVelocity = 750.0f;
    constexpr float kPlayerVelocityScale = 1.25f;
    constexpr float kDefaultEyeHeight = 64.06f;
    constexpr float kViewOffsetMin = 30.0f;
    constexpr float kViewOffsetMax = 70.0f;

    // --- Spawn position assumptions ---
    // Traces forward from eye, then pulls back to avoid spawning inside geometry.
    constexpr float kSpawnTraceForward = 22.0f;
    constexpr float kSpawnPullBack = 6.0f;
    constexpr float kThrowZOffsetScale = 12.0f;

    // --- Detonation times ---
    // Client CGrenadeTracer extends supported grenade horizons by this amount.
    constexpr float kClientTracerHorizonPadding = 0.125f;

    // Regression fuse and safety-cap values.
    constexpr float kDetonateTimeHeFlash = 1.5f;
    constexpr float kDetonateTimeMolotov = 2.0f;
    constexpr float kDetonateTimeDecoy = 10.0f;
    constexpr float kDetonateTimeSmokeCap = 18.0f;

    // --- Panel visual sizes (pixels) ---
    constexpr float kDotPanelCreationSize = 4.0f;
    constexpr float kTrajectoryDotSize = 3.0f;
    constexpr float kBounceDotSize = 8.0f;
    constexpr float kEndMarkerSize = 10.0f;

    // End-position marker (orange) hue
    constexpr auto kEndMarkerHue = color::Hue{30.0f / 360.0f};
}
