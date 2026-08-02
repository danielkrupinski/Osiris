#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>

#include <CS2/Classes/Entities/C_CSPlayerPawn.h>
#include <CS2/Classes/Entities/WeaponEntities.h>
#include <CS2/Offsets/ClientDllOffsets.h>
#include <CS2/Classes/Vector.h>
#include <CS2/Constants/BoneIndex.h>
#include <GameClient/Entities/PlayerPawn.h>
#include <GameClient/EntitySystem/EntitySystem.h>
#include <GameClient/WorldToScreen/WorldToClipSpaceConverter.h>
#include <HookContext/HookContextMacros.h>
#include <Platform/Macros/IsPlatform.h>

#include "AimbotConfigVariables.h"
#include "AimbotParams.h"
#include "AimbotState.h"
#include "SilentAim.h"

#if IS_WIN64()
#include <Windows.h>
#endif

template <typename HookContext>
class Aimbot {
public:
    explicit Aimbot(HookContext& hookContext) noexcept
        : hookContext{hookContext}
        , enabled{GET_CONFIG_VAR(aimbot_vars::Enabled)}
        , smoothMode{GET_CONFIG_VAR(aimbot_vars::Smooth)}
        , humanizationEnabled{GET_CONFIG_VAR(aimbot_vars::Humanization)}
    {
    }

    void checkPlayer(auto&& playerPawn) noexcept
    {
        // Note: deliberately NO early return on bestCandidate.valid — we must
        // walk every enemy to pick the one closest to the crosshair.
        if (!enabled)
            return;
        if (!isGameInForeground())
            return;
        if (!isLocalPlayerInGame())
            return;
        // Hot-key gate: skips aim entirely unless the user is "engaging".
        // Single most effective VAC concealment — angle changes only happen
        // during deliberate input, matching natural player behavior.
        if (!isHotKeyActive())
            return;
        // Movement penalty: rifles/snipers skip while running; SMG/pistol
        // are fine because real players DO run-and-gun with those.
        if (shouldSkipWhileMoving())
            return;
        if (!playerPawn.isAlive().value_or(true))
            return;
        if (playerPawn.isControlledByLocalPlayer())
            return;
        if (!playerPawn.isTTorCT())
            return;
        if (!playerPawn.isEnemy().value_or(true))
            return;
        if (playerPawn.isDormant())
            return;

        const auto origin = playerPawn.absOrigin();
        if (!origin.hasValue())
            return;
        const auto& org = origin.value();

        // The enemy's view offset gives the true eye-height *for this frame*,
        // so we automatically follow crouch/jump/animation. Standing ≈ 64,
        // crouched ≈ 46. This is the fix for "locks above head when ducked".
        const auto enemyViewOff = playerPawn.viewOffset();
        const auto enemyEyeZ = enemyViewOff.z;

        // Velocity prediction: project enemy forward to compensate for
        // the "we aim where the enemy WAS one frame ago" lag. With
        // humanization the lookahead time is randomized each frame to
        // avoid a constant projection fingerprint.
        cs2::Vector predOffset{};
        if (GET_CONFIG_VAR(aimbot_vars::PredictVelocity)) {
            const auto v = playerPawn.absVelocity();
            const auto predTime = aimbot_params::kVelocityPredictionTimeSeconds;
            predOffset = cs2::Vector{
                v.x * predTime,
                v.y * predTime,
                v.z * predTime,
            };
        }

        // Aim target: try bone position (most accurate when valid), fall back
        // to viewOffset-scaled origin (handles crouch correctly without bones).
        // The bone read is sanity-checked because the bone array offset is a
        // community guess — invalid reads return wildly distant coordinates.
        const auto aimPoint = GET_CONFIG_VAR(aimbot_vars::AimPoint);
        cs2::Vector aimPos{};
        {
            const auto boneIdx = boneIndexForAimPoint(aimPoint);
            const auto bonePos = playerPawn.bonePosition(boneIdx);
            bool boneValid = false;
            if (bonePos.hasValue()) {
                const auto& bp = bonePos.value();
                const auto bx = bp.x - org.x;
                const auto by = bp.y - org.y;
                const auto bz = bp.z - org.z;
                // A real bone is within ~100u of the origin; anything farther
                // means the bone array offset is stale and we got garbage.
                if (bx * bx + by * by + bz * bz < 10000.0f) {
                    aimPos = bp;
                    // CS2 bone 6 (Head) sits at the skull base / neck joint.
                    // The head hitbox centre is ~3 world-units above that.
                    // Shift Z up so the lock lands centre-head instead of neck.
                    if (aimPoint == AimbotAimPoint::Head)
                        aimPos.z += kHeadBoneZBias;
                    boneValid = true;
                }
            }
            if (!boneValid) {
                // Fallback: scale aim point by the enemy's current viewOffset.z
                // so Head sits at the true eye height (correct for crouching).
                const auto fraction = aimPointFraction(aimPoint);
                aimPos = cs2::Vector{org.x, org.y, org.z + enemyEyeZ * fraction};
            }
        }
        // Sub-bone spatial offset: shift the lock point by a small random
        // amount so aim trajectories don't converge to the mathematically
        // exact bone coordinate. VACnet trajectory classifiers flag zero-
        // variance convergences as non-human.
        if (humanizationEnabled) {
            aimPos.x += randRange(-aimbot_params::kBoneOffsetMaxUnits, aimbot_params::kBoneOffsetMaxUnits);
            aimPos.y += randRange(-aimbot_params::kBoneOffsetMaxUnits, aimbot_params::kBoneOffsetMaxUnits);
            aimPos.z += randRange(-aimbot_params::kBoneOffsetMaxUnits * 0.4f, aimbot_params::kBoneOffsetMaxUnits * 0.4f);
        }
        // Apply velocity prediction on top of the bone position.
        aimPos.x += predOffset.x;
        aimPos.y += predOffset.y;
        aimPos.z += predOffset.z;

        const auto localPawn = getLocalPlayerPawn();
        if (!localPawn)
            return;
        const auto eyePos = getEyePosition(localPawn);

        // Distance gate uses chest (mid-body).
        const auto chestZ = org.z + kChestZOffset;
        const auto ddx = org.x - eyePos.x;
        const auto ddy = org.y - eyePos.y;
        const auto ddz = chestZ - eyePos.z;
        const auto distSq3d = ddx * ddx + ddy * ddy + ddz * ddz;
        if (distSq3d > kMaxEngagementRangeSq)
            return;

        const auto mode = GET_CONFIG_VAR(aimbot_vars::LockMode);

        if (mode == AimbotLockMode::Closest) {
            // 360° lock: find enemy with smallest angular distance from
            // current view direction. Completely ignores the crosshair —
            // the aimbot can lock enemies behind you or off-screen.
            float angDist = computeAngularDistance(eyePos, aimPos);
            if (angDist < 0.0f) angDist = 180.0f; // safety: behind camera
            // Convert angular distance to a "score" that replaces ndcDistSq
            // in the best-candidate comparison. Smaller angle = better.
            if (!bestCandidate.valid || angDist < bestCandidate.ndcDistSq) {
                bestCandidate.aimPos = aimPos;
                bestCandidate.ndcDistSq = angDist;
                bestCandidate.valid = true;
                bestCandidate.targetPawn = static_cast<const void*>(playerPawn.rawPointer());
            }
        } else {
            // Crosshair mode: existing NDC multi-point body check.
            const auto converter = hookContext.template make<WorldToClipSpaceConverter>();
            const auto ndcRadius = calculateNdcRadius();
            const auto ndcRadiusSq = ndcRadius * ndcRadius;

            const float scaledOffsets[4]{
                enemyEyeZ,
                enemyEyeZ * 0.78f,
                enemyEyeZ * 0.56f,
                enemyEyeZ * 0.31f,
            };
            float closestNdcSq = ndcRadiusSq;
            bool inRange = false;
            for (const auto zOff : scaledOffsets) {
                const cs2::Vector p{org.x + predOffset.x, org.y + predOffset.y, org.z + zOff + predOffset.z};
                const auto clip = converter.toClipSpace(p);
                if (!clip.onScreen())
                    continue;
                const auto ndcX = clip.x / clip.w;
                const auto ndcY = clip.y / clip.w;
                const auto distSq = ndcX * ndcX + ndcY * ndcY;
                if (distSq < closestNdcSq) {
                    closestNdcSq = distSq;
                    inRange = true;
                }
            }

            if (inRange && (!bestCandidate.valid || closestNdcSq < bestCandidate.ndcDistSq)) {
                bestCandidate.aimPos = aimPos;
                bestCandidate.ndcDistSq = closestNdcSq;
                bestCandidate.valid = true;
                bestCandidate.targetPawn = static_cast<const void*>(playerPawn.rawPointer());
            }
        }
    }

    void execute() noexcept
    {
        if (!enabled || !isLocalPlayerAlive() || !isGameInForeground()) {
            bestCandidate.valid = false;
            state().aiming = false;
            state().wasAimingLastFrame = false;
            state().phase = AimbotPhase::Idle;
            state().phaseElapsedSec = 0.0f;
            state().lastTargetPawn = nullptr;
            state().chainKills = 0;
            state().cooldownUntilSec = 0.0f;
            return;
        }

        // Chain-kill cooldown: after kKillCooldown successive engagements,
        // pause for a randomized interval. This breaks the "15 headshots in
        // 3 min" pattern VACnet flags. We treat every valid engagement as a
        // "kill attempt" (slight overcount, but conservative).
        const auto curtimeVal = hookContext.globalVars().curtime();
        const float curtime = curtimeVal.hasValue() ? curtimeVal.value() : 0.0f;
        if (state().cooldownUntilSec > 0.0f) {
            if (curtime < state().cooldownUntilSec) {
                bestCandidate.valid = false;
                state().aiming = false;
                return;
            }
            state().chainKills = 0;
            state().cooldownUntilSec = 0.0f;
        }

        if (bestCandidate.valid) {
            // Lock-break: after consecutive frames locked on the same target,
            // briefly disengage. A never-breaking lock is a dead giveaway.
            if (state().breakFrames > 0) {
                state().breakFrames--;
                bestCandidate.valid = false;
                state().aiming = state().wasAimingLastFrame;
                return;
            }
            const bool sameTarget = bestCandidate.targetPawn == state().lastTargetPawn;
            if (sameTarget) {
                state().lockFrames++;
                const auto limit = static_cast<std::uint8_t>(GET_CONFIG_VAR(aimbot_vars::LockBreakInterval));
                if (state().lockFrames >= limit) {
                    state().lockFrames = 0;
                    state().breakFrames = static_cast<std::uint8_t>(randRange(2.0f, 8.0f));
                }
            } else {
                state().lockFrames = 0;
                state().breakFrames = 0;
            }

            if (humanizationEnabled && rand01() < aimbot_params::kFrameSkipProbability) {
                bestCandidate.valid = false;
                state().aiming = state().wasAimingLastFrame;
                return;
            }

            if (!state().wasAimingLastFrame && humanizationEnabled) {
                state().smoothFactor = randRange(aimbot_params::kSmoothFactorMin,
                                                  aimbot_params::kSmoothFactorMax);
            }

            const auto localPawn = getLocalPlayerPawn();
            if (localPawn) {
                // Miss chance / body diversification: when a new target is
                // acquired, roll once. If we decide to miss, offset the aim
                // by ~body-width so the shot whiffs naturally. VACnet's
                // headshot-ratio classifier can't flag a player whose HR is
                // a human-like 60-70% instead of 98%.
                if (!state().wasAimingLastFrame) {
                    const auto missPct = static_cast<float>(GET_CONFIG_VAR(aimbot_vars::MissChance)) * 0.01f;
                    state().missThisTarget = (rand01() < missPct);
                }

                const auto eyePos = getEyePosition(localPawn);
                float pitch, yaw;
                if (state().missThisTarget) {
                    // Miss: bias the aim point to body-width off the real
                    // target. The player's shot lands near the enemy without
                    // hitting — looks like bad spray control.
                    const cs2::Vector missPos{
                        bestCandidate.aimPos.x + randRange(-25.0f, 25.0f),
                        bestCandidate.aimPos.y + randRange(-25.0f, 25.0f),
                        bestCandidate.aimPos.z + randRange(-15.0f, 5.0f),
                    };
                    computeAimAngle(eyePos, missPos, pitch, yaw);
                } else {
                    computeAimAngle(eyePos, bestCandidate.aimPos, pitch, yaw);
                }

                compensateRecoil(localPawn, pitch, yaw);

                if (humanizationEnabled) {
                    pitch += randRange(-aimbot_params::kAimJitterMaxDegrees,
                                        aimbot_params::kAimJitterMaxDegrees);
                    yaw += randRange(-aimbot_params::kAimJitterMaxDegrees,
                                      aimbot_params::kAimJitterMaxDegrees);
                }

                auto& hooks = hookContext.hooks();
                if (GET_CONFIG_VAR(aimbot_vars::SilentAim)) {
                    if (!aimbot::silent::isAvailable(hooks)) {
                        aimbot::silent::cancelRequest(hooks);
                        bestCandidate.valid = false;
                        bestCandidate.targetPawn = nullptr;
                        state().aiming = false;
                        state().wasAimingLastFrame = false;
                        state().lastTargetPawn = nullptr;
                        state().lockedTargetPawn = nullptr;
                        return;
                    }
                    aimbot::silent::request(hooks, pitch, yaw);
                } else {
                    if (smoothMode)
                        applySmoothAim(pitch, yaw);
                    else
                        applySnapAim(pitch, yaw);
                }
                state().aiming = true;
                if (GET_CONFIG_VAR(aimbot_vars::LockIndicator))
                    state().lockedTargetPawn = bestCandidate.targetPawn;

                // Increment chain-kill counter; cap at configured threshold
                // then start cooldown timer.
                if (!state().wasAimingLastFrame) {
                    state().chainKills++;
                    const auto maxChain = static_cast<std::uint8_t>(
                        GET_CONFIG_VAR(aimbot_vars::KillCooldown));
                    if (state().chainKills >= maxChain) {
                        state().cooldownUntilSec = curtime + randRange(4.0f, 12.0f);
                        state().chainKills = 0;
                    }
                }
            }
        } else {
            state().aiming = false;
            state().phase = AimbotPhase::Idle;
            state().phaseElapsedSec = 0.0f;
            state().lastTargetPawn = nullptr;
            state().lockedTargetPawn = nullptr;
        }

        state().wasAimingLastFrame = bestCandidate.valid;
        state().lastTargetPawn = bestCandidate.valid ? bestCandidate.targetPawn : nullptr;
        bestCandidate.valid = false;
        bestCandidate.targetPawn = nullptr;
    }

private:
    [[nodiscard]] static cs2::BoneIndex boneIndexForAimPoint(AimbotAimPoint aimPoint) noexcept
    {
        switch (aimPoint) {
        case AimbotAimPoint::Head:    return cs2::BoneIndex::Head;
        case AimbotAimPoint::Neck:    return cs2::BoneIndex::Neck;
        case AimbotAimPoint::Chest:   return cs2::BoneIndex::Chest;
        case AimbotAimPoint::Stomach: return cs2::BoneIndex::Stomach;
        case AimbotAimPoint::Pelvis:  return cs2::BoneIndex::Pelvis;
        default:                      return cs2::BoneIndex::Head;
        }
    }

    // Fraction of the enemy's viewOffset.z (eye height) at which to aim.
    // Multiplying by the live viewOffset.z makes the aim point follow crouch
    // and animation automatically — Head 1.0 sits on the actual eye no matter
    // whether the enemy is standing (eye≈64) or crouched (eye≈46).
    [[nodiscard]] static float aimPointFraction(AimbotAimPoint aimPoint) noexcept
    {
        switch (aimPoint) {
        case AimbotAimPoint::Head:    return 1.00f;
        case AimbotAimPoint::Neck:    return 0.92f;
        case AimbotAimPoint::Chest:   return 0.78f;
        case AimbotAimPoint::Stomach: return 0.56f;
        case AimbotAimPoint::Pelvis:  return 0.31f;
        default:                      return 1.00f;
        }
    }

    [[nodiscard]] bool isHotKeyActive() const noexcept
    {
        const auto key = GET_CONFIG_VAR(aimbot_vars::HotKey);
        if (key == AimbotKey::Always)
            return true;
#if IS_WIN64()
        switch (key) {
        case AimbotKey::RightMouse: return (GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0;
        case AimbotKey::Mouse5:     return (GetAsyncKeyState(VK_XBUTTON2) & 0x8000) != 0;
        case AimbotKey::Shift:      return (GetAsyncKeyState(VK_SHIFT)    & 0x8000) != 0;
        case AimbotKey::CapsLock:   return (GetKeyState(VK_CAPITAL) & 0x0001) != 0;
        default:                    return true;
        }
#else
        return true;
#endif
    }

    void computeAimAngle(const cs2::Vector& eye, const cs2::Vector& target, float& pitch, float& yaw) noexcept
    {
        const auto deltaX = target.x - eye.x;
        const auto deltaY = target.y - eye.y;
        const auto deltaZ = target.z - eye.z;
        constexpr auto kRadToDeg = 57.295779513082f;
        yaw = fastAtan2(deltaY, deltaX) * kRadToDeg;
        const auto hyp = fastSqrt(deltaX * deltaX + deltaY * deltaY);
        pitch = -fastAtan2(deltaZ, hyp) * kRadToDeg;
        clampAngles(pitch, yaw);
    }

    void compensateRecoil(cs2::C_CSPlayerPawn* localPawn, float& pitch, float& yaw) const noexcept
    {
#if IS_WIN64()
        // C_CSPlayerPawn::m_aimPunchAngle is the authoritative weapon-recoil
        // punch that the game actually uses for bullet trajectory. It is
        // (0,0,0) when not firing. Do NOT use CameraServices::
        // m_vecCsViewPunchAngle here — that field includes camera shake and
        // rendering offsets that are non-zero even between shots, which makes
        // the lock point drift above the target.
        if (!isReadableMemory(localPawn, kAimPunchAngleOffset + sizeof(cs2::Vector)))
            return;

        const auto punch = *reinterpret_cast<const cs2::Vector*>(
            reinterpret_cast<const std::byte*>(localPawn) + kAimPunchAngleOffset);
        // Reject a stale pointer or a transient invalid schema read.
        if (punch.x <= -45.0f || punch.x >= 45.0f
            || punch.y <= -45.0f || punch.y >= 45.0f)
            return;

        pitch -= punch.x * kWeaponRecoilScale;
        yaw -= punch.y * kWeaponRecoilScale;
        clampAngles(pitch, yaw);
#else
        (void)localPawn;
        (void)pitch;
        (void)yaw;
#endif
    }

    static void clampAngles(float& pitch, float& yaw) noexcept
    {
        if (pitch > 89.0f) pitch = 89.0f;
        if (pitch < -89.0f) pitch = -89.0f;
        while (yaw > 180.0f) yaw -= 360.0f;
        while (yaw < -180.0f) yaw += 360.0f;
    }

    // Angular distance (in degrees) between current view direction and the
    // enemy target. Used by Closest lock mode to pick the nearest enemy by
    // angle regardless of crosshair position (360° awareness).
    [[nodiscard]] float computeAngularDistance(const cs2::Vector& eye, const cs2::Vector& target) noexcept
    {
        float tgtPitch, tgtYaw;
        computeAimAngle(eye, target, tgtPitch, tgtYaw);
        ensureClientBase();
        if (!m_clientBase)
            return 180.0f;
        auto* angles = globalViewAngles();
        const auto curPitch = angles[0];
        auto curYaw = angles[1];
        auto dPitch = tgtPitch - curPitch;
        auto dYaw = tgtYaw - curYaw;
        while (dYaw > 180.0f) dYaw -= 360.0f;
        while (dYaw < -180.0f) dYaw += 360.0f;
        return fastSqrt(dPitch * dPitch + dYaw * dYaw);
    }

    void applySnapAim(float pitchTarget, float yawTarget) noexcept
    {
#if IS_WIN64()
        ensureClientBase();
        if (!m_clientBase)
            return;
        auto* angles = globalViewAngles();
        const auto currentPitch = angles[0];
        const auto currentYaw = angles[1];

        auto pitchDelta = pitchTarget - currentPitch;
        auto yawDelta = yawTarget - currentYaw;
        while (yawDelta > 180.0f) yawDelta -= 360.0f;
        while (yawDelta < -180.0f) yawDelta += 360.0f;

        // Cap snap delta only when the crosshair IS visible (non-silent).
        // In silent-aim mode the crosshair never moves, so VACnet has no
        // angle curve to flag — full-angle write is safe AND required for
        // 360° lock to work in one frame (otherwise delta-cap causes shots
        // to land mid-arc instead of on-target).
        if (humanizationEnabled && !GET_CONFIG_VAR(aimbot_vars::SilentAim)) {
            const auto cap = aimbot_params::kMaxAngleDeltaPerFrame;
            pitchDelta = std::clamp(pitchDelta, -cap, cap);
            yawDelta = std::clamp(yawDelta, -cap, cap);
        }

        auto newPitch = currentPitch + pitchDelta;
        auto newYaw = currentYaw + yawDelta;

        // Tremor overlay — even in snap mode we want continuous low-amplitude
        // motion to avoid the "frozen sample" pattern XGuardian flags. Phase
        // step uses real dt so tremor frequency stays constant across
        // framerates.
        if (humanizationEnabled) {
            auto& s = state();
            const float dt = computeDeltaTime(s);
            s.tremorPhase += aimbot_params::kTremorPhaseSpeedRadPerSec * dt;
            const float amp = aimbot_params::kTremorAmpMoving;
            newPitch += amp * smoothTremor(s.tremorPhase, 0);
            newYaw   += amp * smoothTremor(s.tremorPhase, 1);
        }

        clampAngles(newPitch, newYaw);

        angles[0] = newPitch;
        angles[1] = newYaw;
        angles[2] = 0.0f;
        syncVisibleAngles(newPitch, newYaw);
#else
        (void)pitchTarget; (void)yawTarget;
#endif
    }

    // Human-like smooth aim driven by the Woodworth two-component motor
    // model (Reacting → Ballistic → Corrective → Hold). Inside each phase we
    // shape the trajectory so the pitch/yaw velocity profile matches what an
    // XGuardian-style GRU-CNN trained on real player traces expects to see:
    // bell-curve velocity during ballistic, overshoot-then-correct, slow
    // closing in corrective, and continuous low-amplitude tremor throughout.
    void applySmoothAim(float pitchTarget, float yawTarget) noexcept
    {
#if IS_WIN64()
        ensureClientBase();
        if (!m_clientBase)
            return;
        auto* angles = globalViewAngles();
        const auto currentPitch = angles[0];
        const auto currentYaw = angles[1];

        auto& s = state();
        const float dt = computeDeltaTime(s);

        // New engagement triggers when either (a) we weren't aiming last frame
        // or (b) the closest-to-crosshair enemy changed identity. Restart on
        // both so the new target gets a fresh Reacting→Ballistic sequence.
        const bool targetSwitched =
            bestCandidate.targetPawn != s.lastTargetPawn;
        if (!s.wasAimingLastFrame || targetSwitched) {
            beginEngagement(s, currentPitch, currentYaw, pitchTarget, yawTarget);
        }

        s.phaseElapsedSec += dt;

        float renderPitch = currentPitch;
        float renderYaw = currentYaw;
        runPhase(s, dt, currentPitch, currentYaw, pitchTarget, yawTarget, renderPitch, renderYaw);

        // Tremor overlay — continuous low-amplitude shake. Even on Hold phase
        // we never settle at exact bone coordinate; XGuardian flags "frozen"
        // single-frame aim as non-human. Phase speed scales with real dt so
        // tremor frequency is constant regardless of framerate.
        if (humanizationEnabled) {
            s.tremorPhase += aimbot_params::kTremorPhaseSpeedRadPerSec * dt;
            const float amp = (s.phase == AimbotPhase::Hold)
                ? aimbot_params::kTremorAmpHold
                : aimbot_params::kTremorAmpMoving;
            renderPitch += amp * smoothTremor(s.tremorPhase, 0);
            renderYaw   += amp * smoothTremor(s.tremorPhase, 1);
        }

        clampAngles(renderPitch, renderYaw);
        angles[0] = renderPitch;
        angles[1] = renderYaw;
        angles[2] = 0.0f;
        syncVisibleAngles(renderPitch, renderYaw);
#else
        (void)pitchTarget; (void)yawTarget;
#endif
    }

    // Walls-clock delta from the engine's curtime() global. Falls back to a
    // sane 60fps value if curtime is unavailable, zero, or has jumped weirdly
    // (e.g., long hitch, menu pause, level transition).
    [[nodiscard]] float computeDeltaTime(AimbotState& s) const noexcept
    {
        const auto curOpt = hookContext.globalVars().curtime();
        if (!curOpt.hasValue())
            return aimbot_params::kFallbackDtSec;
        const float currentTime = curOpt.value();
        float dt = currentTime - s.lastCurtime;
        if (currentTime <= 0.0f
            || dt < aimbot_params::kMinValidDtSec
            || dt > aimbot_params::kMaxValidDtSec) {
            dt = aimbot_params::kFallbackDtSec;
        }
        s.lastCurtime = currentTime;
        return dt;
    }

    [[nodiscard]] static float scaleLerpFactor(float refFactor60, float dt) noexcept
    {
        const float scaled = refFactor60 * dt * aimbot_params::kReferenceFps;
        return scaled < 0.0f ? 0.0f
             : scaled > aimbot_params::kLerpFactorMaxCap ? aimbot_params::kLerpFactorMaxCap
             : scaled;
    }

    void beginEngagement(AimbotState& s, float curP, float curY, float tgtP, float tgtY) noexcept
    {
        s.phase = humanizationEnabled ? AimbotPhase::Reacting : AimbotPhase::Ballistic;
        s.phaseElapsedSec = 0.0f;
        s.startPitch = curP;
        s.startYaw = curY;

        if (humanizationEnabled) {
            s.reactionTime = randRange(aimbot_params::kReactionTimeMinSec,
                                        aimbot_params::kReactionTimeMaxSec);
            s.ballisticTime = randRange(aimbot_params::kBallisticTimeMinSec,
                                         aimbot_params::kBallisticTimeMaxSec);
            s.correctiveTime = randRange(aimbot_params::kCorrectiveTimeMinSec,
                                          aimbot_params::kCorrectiveTimeMaxSec);

            const float dP = tgtP - curP;
            const float dY = normalizeYawDelta(tgtY - curY);
            const float frac = randRange(aimbot_params::kOvershootFractionMin,
                                          aimbot_params::kOvershootFractionMax);
            s.overshootPitch = dP * frac;
            s.overshootYaw   = dY * frac;
        } else {
            s.reactionTime = 0.0f;
            s.ballisticTime = 0.100f;
            s.correctiveTime = 0.050f;
            s.overshootPitch = 0.0f;
            s.overshootYaw = 0.0f;
        }
    }

    void runPhase(AimbotState& s, float dt, float curP, float curY, float tgtP, float tgtY,
                   float& outP, float& outY) noexcept
    {
        // Each `if` is a phase block. When a block completes, we subtract its
        // duration from phaseElapsedSec (carry over the excess) and fall
        // through to the next phase. This means the first frame after Reacting
        // already produces a small Ballistic movement instead of wasting a
        // frame sitting at tau=0.
        if (s.phase == AimbotPhase::Idle || s.phase == AimbotPhase::Reacting) {
            if (s.phaseElapsedSec < s.reactionTime) {
                outP = curP;
                outY = curY;
                return;
            }
            s.phaseElapsedSec -= s.reactionTime;
            s.phase = AimbotPhase::Ballistic;
            // Refresh ballistic start from CURRENT angles (user may have
            // moved their view during the reaction window).
            s.startPitch = curP;
            s.startYaw = curY;
            // fall through to Ballistic
        }

        if (s.phase == AimbotPhase::Ballistic) {
            if (s.phaseElapsedSec < s.ballisticTime) {
                const float denom = s.ballisticTime > 0.0f ? s.ballisticTime : 1.0f;
                const float tau = s.phaseElapsedSec / denom;
                const float pos = minimumJerk(tau);
                const float over = overshootEnvelope(tau);
                const float dP = tgtP - s.startPitch;
                const float dY = normalizeYawDelta(tgtY - s.startYaw);
                outP = s.startPitch + dP * pos + s.overshootPitch * over;
                outY = s.startYaw   + dY * pos + s.overshootYaw   * over;
                return;
            }
            s.phaseElapsedSec -= s.ballisticTime;
            s.phase = AimbotPhase::Corrective;
            // fall through to Corrective
        }

        if (s.phase == AimbotPhase::Corrective) {
            // Feedback closure — framerate-normalized exponential lerp.
            const float dP = tgtP - curP;
            const float dY = normalizeYawDelta(tgtY - curY);
            const float f = scaleLerpFactor(aimbot_params::kCorrectiveLerpFactor60, dt);
            outP = curP + dP * f;
            outY = curY + dY * f;
            if (s.phaseElapsedSec >= s.correctiveTime) {
                s.phaseElapsedSec -= s.correctiveTime;
                s.phase = AimbotPhase::Hold;
            }
            return;
        }

        // Hold — framerate-normalized factor so strafing targets get the
        // same closure rate at any framerate.
        const float dP = tgtP - curP;
        const float dY = normalizeYawDelta(tgtY - curY);
        const float f = scaleLerpFactor(aimbot_params::kHoldLerpFactor60, dt);
        outP = curP + dP * f;
        outY = curY + dY * f;
    }

    void writeViewAngles(float pitch, float yaw) noexcept
    {
#if IS_WIN64()
        ensureClientBase();
        if (!m_clientBase)
            return;
        auto* viewAngles = globalViewAngles();
        viewAngles[0] = pitch;
        viewAngles[1] = yaw;
        viewAngles[2] = 0.0f;
        syncVisibleAngles(pitch, yaw);
#else
        (void)pitch; (void)yaw;
#endif
    }

    void syncVisibleAngles(float pitch, float yaw) noexcept
    {
        const auto localPawn = getLocalPlayerPawn();
        if (!localPawn)
            return;
        // Write ALL THREE angle fields every time. CS2 syncs v_angle →
        // dwViewAngles at end-of-render; if v_angle isn't updated, our
        // dwViewAngles write gets clobbered and CreateMove sees the old
        // angle (bullet hits wrong spot). Writing v_angle causes ~1 frame
        // of world rotation (~8ms @ 120fps, imperceptible) but guarantees
        // the server sees the correct shot.
        //
        // Visible aim needs both fields so camera, crosshair, and command
        // construction stay in sync. Silent aim must not call this path.
        float* v_angle = reinterpret_cast<float*>(
            reinterpret_cast<std::byte*>(localPawn) + kViewAngleOffset);
        v_angle[0] = pitch;
        v_angle[1] = yaw;
        float* eyeAngles = reinterpret_cast<float*>(
            reinterpret_cast<std::byte*>(localPawn) + kEyeAnglesOffset);
        eyeAngles[0] = pitch;
        eyeAngles[1] = yaw;
    }

    void ensureClientBase() noexcept
    {
#if IS_WIN64()
        if (!m_clientBase)
            m_clientBase = GetModuleHandleA("client.dll");
#endif
    }

    [[nodiscard]] float* globalViewAngles() noexcept
    {
#if IS_WIN64()
        return reinterpret_cast<float*>(
            reinterpret_cast<std::byte*>(m_clientBase) + cs2::client_dll_offsets::kViewAngles);
#else
        return nullptr;
#endif
    }

    [[nodiscard]] static float fastFabs(float x) noexcept { return x < 0.0f ? -x : x; }

    [[nodiscard]] static float fastAtan2(float y, float x) noexcept
    {
        const auto ax = fastFabs(x);
        const auto ay = fastFabs(y);
        if (ax < 0.000001f && ay < 0.000001f)
            return 0.0f;
        const auto a = (ax < ay) ? (ax / ay) : (ay / ax);
        const auto s = a * a;
        auto r = ((-0.0464964749f * s + 0.15931422f) * s - 0.327622764f) * s * a + a;
        if (ay > ax) r = 1.57079637f - r;
        if (x < 0.0f) r = 3.14159274f - r;
        if (y < 0.0f) r = -r;
        return r;
    }

    [[nodiscard]] static float fastSqrt(float x) noexcept
    {
        if (x <= 0.0f) return 0.0f;
        std::int32_t i;
        float x2, y;
        x2 = x * 0.5f;
        y = x;
        i = *reinterpret_cast<std::int32_t*>(&y);
        i = 0x5f3759df - (i >> 1);
        y = *reinterpret_cast<float*>(&i);
        y = y * (1.5f - (x2 * y * y));
        y = y * (1.5f - (x2 * y * y));
        return 1.0f / y;
    }

    // CRT-free sin. Bhaskara approximation, max error ~0.0016 over [-pi,pi].
    [[nodiscard]] static float fastSin(float x) noexcept
    {
        constexpr float kTwoPi = 6.28318531f;
        constexpr float kInvTwoPi = 0.15915494f;
        // Wrap to [-pi,pi] without std::floor.
        const auto k = static_cast<std::int32_t>(x * kInvTwoPi + (x < 0.0f ? -0.5f : 0.5f));
        x = x - kTwoPi * static_cast<float>(k);
        const float x2 = x * x;
        return x * (1.0f - x2 * (0.16605f - x2 * 0.00761f));
    }

    // Multi-harmonic noise resembling 1/f hand tremor. Smoothly varying with
    // phase so consecutive frames have correlated noise (unlike uniform random
    // which is statistically detectable as white noise).
    [[nodiscard]] static float smoothTremor(float phase, int axis) noexcept
    {
        const float p = phase + static_cast<float>(axis) * 1.73f;
        return 0.50f * fastSin(p * 3.10f)
             + 0.30f * fastSin(p * 7.31f + 0.70f)
             + 0.20f * fastSin(p * 13.07f + 1.31f);
    }

    // Minimum-jerk position curve: s(τ) = 10τ³ - 15τ⁴ + 6τ⁵.
    // Produces a bell-shaped velocity profile (Hogan, 1984) that matches
    // human reaching motion. τ in [0,1].
    [[nodiscard]] static float minimumJerk(float tau) noexcept
    {
        if (tau <= 0.0f) return 0.0f;
        if (tau >= 1.0f) return 1.0f;
        const float t3 = tau * tau * tau;
        return t3 * (10.0f - 15.0f * tau + 6.0f * tau * tau);
    }

    // Symmetric overshoot envelope: peaks at tau=0.6, settles at endpoints.
    // Human flicks typically overshoot most around 60-70% into the movement.
    [[nodiscard]] static float overshootEnvelope(float tau) noexcept
    {
        if (tau <= 0.0f || tau >= 1.0f) return 0.0f;
        // tent-shaped curve peaking near 0.65 then decaying to 0
        const float a = tau / 0.65f;
        const float b = (1.0f - tau) / 0.35f;
        return (a < b ? a : b);
    }

    [[nodiscard]] static float normalizeYawDelta(float d) noexcept
    {
        while (d > 180.0f) d -= 360.0f;
        while (d < -180.0f) d += 360.0f;
        return d;
    }

    // ---- PRNG for humanization noise (xorshift32, no <random> dependency) ----
    [[nodiscard]] static std::uint32_t randU32() noexcept
    {
        static std::uint32_t s = 0x92D68CA2;
        s ^= s << 13;
        s ^= s >> 17;
        s ^= s << 5;
        return s;
    }

    [[nodiscard]] static float rand01() noexcept
    {
        return static_cast<float>(randU32() & 0xFFFFFF) / 16777216.0f;
    }

    [[nodiscard]] static float randRange(float lo, float hi) noexcept
    {
        return lo + rand01() * (hi - lo);
    }

    [[nodiscard]] cs2::C_CSPlayerPawn* getLocalPlayerPawn() const noexcept
    {
        auto&& localController = hookContext.localPlayerController();
        if (localController) {
            const auto handle = localController.playerPawnHandle();
            if (handle) {
                auto* const pawn = static_cast<cs2::C_CSPlayerPawn*>(
                    hookContext.template make<EntitySystem>().getEntityFromHandle(*handle));
                if (isReadableMemory(pawn, kLocalPawnReadableSize))
                    return pawn;
            }
        }

#if IS_WIN64()
        // The controller-to-pawn handle can lag or be unavailable after a
        // client update. cs2-dumper exposes the authoritative local pawn
        // pointer, so use it as a guarded fallback.
        if (const auto clientDll = GetModuleHandleA("client.dll")) {
            auto* const pawnPointer = reinterpret_cast<cs2::C_CSPlayerPawn**>(
                reinterpret_cast<std::byte*>(clientDll) + cs2::client_dll_offsets::kLocalPlayerPawn);
            if (isReadableMemory(pawnPointer, sizeof(*pawnPointer))) {
                auto* const pawn = *pawnPointer;
                if (isReadableMemory(pawn, kLocalPawnReadableSize))
                    return pawn;
            }
        }
#endif
        return nullptr;
    }

    [[nodiscard]] static bool isReadableMemory(const void* address, std::size_t size) noexcept
    {
#if IS_WIN64()
        if (!address || size == 0)
            return false;

        MEMORY_BASIC_INFORMATION info{};
        if (!VirtualQuery(address, &info, sizeof(info)))
            return false;
        if (info.State != MEM_COMMIT || (info.Protect & (PAGE_GUARD | PAGE_NOACCESS)) != 0)
            return false;

        const auto start = reinterpret_cast<std::uintptr_t>(address);
        const auto end = start + size;
        const auto regionEnd = reinterpret_cast<std::uintptr_t>(info.BaseAddress) + info.RegionSize;
        return end > start && end <= regionEnd;
#else
        return address != nullptr;
#endif
    }

    [[nodiscard]] cs2::Vector getEyePosition(cs2::C_CSPlayerPawn* pawn) const noexcept
    {
        const auto playerPawn = hookContext.template make<PlayerPawn>(pawn);
        const auto origin = playerPawn.absOrigin();
        if (!origin.hasValue())
            return {};
        auto eyePosition = origin.value();
        // Use the dynamic m_vecViewOffset so crouching, jumping, and animation
        // ducks all give the *real* eye height (instead of fixed +64).
        const auto viewOffset = playerPawn.viewOffset();
        eyePosition.x += viewOffset.x;
        eyePosition.y += viewOffset.y;
        eyePosition.z += viewOffset.z;
        return eyePosition;
    }

    [[nodiscard]] bool isGameInForeground() const noexcept
    {
#if IS_WIN64()
        const auto fgWindow = GetForegroundWindow();
        if (!fgWindow)
            return false;
        DWORD fgProcessId = 0;
        GetWindowThreadProcessId(fgWindow, &fgProcessId);
        return fgProcessId == GetCurrentProcessId();
#else
        return true;
#endif
    }

    [[nodiscard]] bool isLocalPlayerInGame() const noexcept
    {
        auto&& localController = hookContext.localPlayerController();
        return static_cast<bool>(localController);
    }

    [[nodiscard]] bool isLocalPlayerAlive() const noexcept
    {
        auto&& localController = hookContext.localPlayerController();
        if (!localController)
            return false;
        const auto isAlive = localController.isPawnAlive();
        return isAlive.value_or(false);
    }

    [[nodiscard]] float calculateNdcRadius() const noexcept
    {
        const auto configRadius = static_cast<float>(GET_CONFIG_VAR(aimbot_vars::FovRadius));
#if IS_WIN64()
        const auto screenWidth = GetSystemMetrics(SM_CXSCREEN);
        if (screenWidth <= 0)
            return (2.0f * configRadius) / aimbot_params::kReferenceScreenWidth;
        // Pixel value defined on the reference 1920px-wide screen, scaled to
        // the actual resolution so the same value gives roughly the same
        // visible circle across monitors.
        return (2.0f * configRadius) / static_cast<float>(screenWidth);
#else
        return (2.0f * configRadius) / aimbot_params::kReferenceScreenWidth;
#endif
    }

    [[nodiscard]] AimbotState& state() const noexcept
    {
        return hookContext.featuresStates().aimbotState;
    }

    static constexpr std::ptrdiff_t kViewAngleOffset{0x12C0};
    static constexpr std::ptrdiff_t kEyeAnglesOffset{0x3340};
    // CS2 head bone (index 6) is the skull-base / neck joint, ~3 units below
    // the head-hitbox centre. Lift aim so shots land centre-head, not neck.
    static constexpr float kHeadBoneZBias{3.0f};
    // C_CSPlayerPawn::m_aimPunchAngle — the authoritative weapon-recoil
    // punch (QAngle at 3 floats), stable at 0x1584 across CS2 builds since
    // mid-2025. This is what the game uses for bullet trajectory; it is
    // (0,0,0) when not firing.
    static constexpr std::ptrdiff_t kAimPunchAngleOffset{0x1584};
    static constexpr float kWeaponRecoilScale{2.0f};
    // Speed² threshold for "running". CS2 walk speed ~130u/s, run ~250u/s.
    // Gate at 140² ≈ 19600 separates walk/crouch from full run.
    static constexpr float kRunSpeedThresholdSq{19600.0f};
    static constexpr std::size_t kLocalPawnReadableSize{0x3350};
    // ===== Movement penalty + weapon awareness =====
    [[nodiscard]] bool shouldSkipWhileMoving() const noexcept
    {
        const auto mode = GET_CONFIG_VAR(aimbot_vars::MovementPenalty);
        if (mode == AimbotMovementPenalty::Off)
            return false;
        if (mode == AimbotMovementPenalty::All && isLocalPlayerMoving())
            return true;
        // Rifles mode: only skip if holding a rifle/sniper AND moving
        if (mode == AimbotMovementPenalty::Rifles && isHoldingRifle() && isLocalPlayerMoving())
            return true;
        return false;
    }

    [[nodiscard]] bool isLocalPlayerMoving() const noexcept
    {
        const auto pawn = getLocalPlayerPawn();
        if (!pawn)
            return false;
        const auto v = hookContext.template make<PlayerPawn>(pawn).absVelocity();
        const auto speedSq = v.x * v.x + v.y * v.y;
        return speedSq > kRunSpeedThresholdSq;
    }

    [[nodiscard]] bool isHoldingRifle() const noexcept
    {
        const auto pawn = getLocalPlayerPawn();
        if (!pawn)
            return true;

        const auto weaponType = hookContext.template make<PlayerPawn>(pawn)
            .getActiveWeapon().baseEntity().classify();
        return weaponType.template is<cs2::C_WeaponGalilAR>()
            || weaponType.template is<cs2::C_WeaponFamas>()
            || weaponType.template is<cs2::C_AK47>()
            || weaponType.template is<cs2::C_WeaponM4A1>()
            || weaponType.template is<cs2::C_WeaponSG556>()
            || weaponType.template is<cs2::C_WeaponAug>()
            || weaponType.template is<cs2::C_WeaponSSG08>()
            || weaponType.template is<cs2::C_WeaponAWP>()
            || weaponType.template is<cs2::C_WeaponG3SG1>()
            || weaponType.template is<cs2::C_WeaponSCAR20>();
    }

    // Body z-offsets above absOrigin (feet) for the multi-point hit test.
    static constexpr float kHeadZOffset{64.0f};
    static constexpr float kChestZOffset{50.0f};
    static constexpr float kBodyZOffsets[4]{64.0f, 50.0f, 36.0f, 20.0f};
    static constexpr float kMaxEngagementRangeSq{3500.0f * 3500.0f};
    // Smooth aim: fraction of remaining angle to cover each frame. 0.3 converges
    // to target in ~5-10 frames (~80-160ms at 60fps) which looks human-ish.
    static constexpr float kSmoothFactor{0.3f};

    struct AimCandidate {
        cs2::Vector aimPos{};
        float ndcDistSq{0.0f};
        bool valid{false};
        // Stable identity of the enemy this candidate corresponds to. Lets
        // the phase machine detect "target switched between frames" and
        // restart the engagement.
        const void* targetPawn{nullptr};
    };

    HookContext& hookContext;
    bool enabled;
    bool smoothMode;
    bool humanizationEnabled;
    AimCandidate bestCandidate{};
#if IS_WIN64()
    HMODULE m_clientBase{nullptr};
#else
    void* m_clientBase{nullptr};
#endif
};
