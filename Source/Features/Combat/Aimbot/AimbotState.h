#pragma once

#include <cstdint>

// Phases of human-like aim engagement, driven by the Woodworth two-component
// motor-control model (ballistic phase + feedback-corrective phase).
// XGuardian's GRU-CNN on pitch/yaw velocity profiles flags clean exponential
// lerps; the phase machine produces minimum-jerk velocity bell curves with
// overshoot+correction patterns that match human motor traces.
enum class AimbotPhase : std::uint8_t {
    Idle = 0,
    Reacting,   // ~120-200ms visual reaction + motor preparation, no movement
    Ballistic,  // ~100-170ms fast open-loop reach with end-of-phase overshoot
    Corrective, // ~50-100ms feedback closure of the overshoot residual
    Hold,       // settled on target, micro-tracking with tremor only
};

struct AimbotState {
    bool aiming{false};
    float smoothFactor{0.3f};      // legacy: kept for snap-mode delta cap
    bool wasAimingLastFrame{false}; // for target-switch detection

    // Phase machine state for human-like smooth aim. Times are in seconds —
    // framerate-independent so the engagement timeline is identical at 60fps
    // and 144fps.
    AimbotPhase phase{AimbotPhase::Idle};
    float phaseElapsedSec{0.0f};
    float reactionTime{0.150f};       // randomized per engagement
    float ballisticTime{0.140f};
    float correctiveTime{0.080f};

    float startPitch{0.0f};        // angles at engagement start
    float startYaw{0.0f};
    float overshootPitch{0.0f};    // end-of-ballistic overshoot magnitude
    float overshootYaw{0.0f};

    // Multi-harmonic tremor phase accumulator (~8Hz hand vibration).
    float tremorPhase{0.0f};

    // Real-time delta computation — diff curtime() across frames so phase
    // durations are in true wall-clock seconds, not frame counts.
    float lastCurtime{0.0f};

    // Stable pointer to the currently locked-on enemy pawn. When the closest-
    // to-crosshair enemy changes between frames, we restart the phase machine
    // so the new engagement gets its own Reacting / Ballistic / Corrective
    // sequence instead of drifting over from the previous target.
    const void* lastTargetPawn{nullptr};

    // Chain-kill tracking for VAC cooldown. DM spawns produce 10+ kills in
    // two minutes — VACnet flags any account whose headshot rate stays >90%
    // for that duration. Counting every engage as a "kill attempt" and
    // pausing after the configured threshold breaks the signature.
    std::uint8_t chainKills{0};
    float cooldownUntilSec{0.0f};
    // Per-engagement miss decision.
    bool missThisTarget{false};
    // Lock-break tracking: consecutive frames locked on same target.
    std::uint8_t lockFrames{0};
    std::uint8_t breakFrames{0};
    // Raw pointer to the enemy pawn currently locked onto. Set by
    // Aimbot::execute(), read by the glow system to highlight the target.
    const void* lockedTargetPawn{nullptr};
};
