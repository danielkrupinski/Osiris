#pragma once

#include <algorithm>
#include <CS2/Classes/Vector.h>
#include <GameClient/EngineTrace/EngineTrace.h>
#include <Utils/Math.h>
#include <Features/Visuals/GrenadePrediction/GrenadeKind.h>
#include <Features/Visuals/GrenadePrediction/Trajectory.h>
#include <Features/Visuals/GrenadePrediction/GrenadePredictionParams.h>

// Return value from a single simulation step.
struct StepResult {
    bool traceSucceeded = true;   // false means no prediction can be produced
    bool impactDetonate = false; // grenade should explode on this tick (molotov flat surface)
    bool hit = false;            // collision occurred
    cs2::Vector hitPos{};        // first exact surface contact point
    int contactsCount{};
    cs2::Vector contacts[grenade_prediction_params::kMovementSubsteps]{};
                                 // NOTE: pos is advanced past this by remaining-fraction
                                 // motion inside resolveCollision, so hitPos != pos after step.
};

template <typename HookContext>
struct GrenadeSimulatorTestAccess;

template <typename HookContext>
class GrenadeSimulator {
public:
    explicit GrenadeSimulator(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    void simulate(Trajectory& traj, cs2::Vector start, cs2::Vector velocity, cs2::GrenadeKind kind, void* skipEntity) noexcept
    {
        using namespace grenade_prediction_params;

        traj.pointsCount = 0;
        traj.bouncesCount = 0;
        traj.endPos = start;
        traj.valid = false;
        traj.validLanding = true;

        cs2::Vector pos = start;
        cs2::Vector vel = velocity;
        int bounceCount = 0;
        int tickTimer = 0;
        bool landedOnSurface = false; // tracks if Molotov ever hit a valid flat surface

        for (int tick = 0; tick < kMaxTicks; ++tick) {
            if (tickTimer == 0) {
                if (traj.pointsCount < 500)
                    traj.points[traj.pointsCount++] = pos;
            }

            cs2::Vector prevPos = pos;
            StepResult result = step(pos, vel, kind, skipEntity);

            // An empty Optional is a trace failure, not a clear trace. Never retain
            // partial points as a valid trajectory when any required trace failed.
            if (!result.traceSucceeded) {
                traj.pointsCount = 0;
                traj.bouncesCount = 0;
                traj.endPos = start;
                traj.validLanding = true;
                return;
            }

            for (int contact = 0; contact < result.contactsCount; ++contact) {
                ++bounceCount;
                if (traj.bouncesCount < 20)
                    traj.bounces[traj.bouncesCount++] = result.contacts[contact];
            }
            if (result.impactDetonate)
                landedOnSurface = true;

            // Regression displacement handling checks per-tick displacement
            // against FLT_EPSILON — if the grenade moved less than ~1.19e-7 units,
            // it's considered stopped. This replaces the old velocity-threshold check
            // and applies to Smoke/Decoy (which deploy when stationary).
            bool displacementStopped = false;
            if (kind == cs2::GrenadeKind::SmokeGrenade || kind == cs2::GrenadeKind::Decoy) {
                cs2::Vector disp = pos - prevPos;
                displacementStopped = disp.squareLength() < kStopDisplacementSq;
            }

            if (result.impactDetonate || shouldDetonate(kind, tick) || bounceCount > kMaxBounces || displacementStopped) {
                traj.endPos = pos;
                traj.valid = true;

                // Molotov: only valid if it actually hit a flat surface. If it timed out
                // or stopped mid-air, the fire won't spread to the ground — hide marker.
                if (kind == cs2::GrenadeKind::Molotov || kind == cs2::GrenadeKind::Incendiary)
                    traj.validLanding = landedOnSurface || result.impactDetonate;

                break;
            }

            if (result.hit || ++tickTimer >= kTicksPerPoint) {
                tickTimer = 0;
            }
        }

        if (traj.valid && traj.pointsCount > 0 && traj.points[traj.pointsCount - 1].squareDistTo(traj.endPos) > 1.0f) {
            if (traj.pointsCount < 500)
                traj.points[traj.pointsCount++] = traj.endPos;
        }
    }

    static cs2::Vector computeInitialVelocity(cs2::Vector viewAngles, float baseVelocity, float throwStrength) noexcept
    {
        float strength = normalizeThrowStrength(throwStrength);

        float pitch = viewAngles.x;
        float yaw = viewAngles.y;

        float correctedPitch = pitch - (90.0f - Math::abs(pitch)) * 10.0f / 90.0f;

        float throwVelocity = std::clamp(baseVelocity * 0.9f, 15.0f, 750.0f);
        float throwSpeed = (strength * 0.7f + 0.3f) * throwVelocity;

        return forwardFromAngles(correctedPitch, yaw) * throwSpeed;
    }

    // Spawn trace regression behavior: traces forward from eye position,
    // then pulls back from hit point to avoid spawning inside geometry.
    // When no collision occurs, this equals eyePos + forward * (kSpawnTraceForward - kSpawnPullBack).
    [[nodiscard]] Optional<cs2::Vector> computeSpawnPosition(cs2::Vector eyePos, cs2::Vector viewAngles, float throwStrength, void* skipEntity) noexcept
    {
        using namespace grenade_prediction_params;

        float strength = normalizeThrowStrength(throwStrength);

        float pitch = viewAngles.x;
        float yaw = viewAngles.y;
        float correctedPitch = pitch - (90.0f - Math::abs(pitch)) * 10.0f / 90.0f;

        cs2::Vector forward = forwardFromAngles(correctedPitch, yaw);

        // Apply Z offset based on throw strength (IDA: eyePos.z += throwStrength * 12.0 - 12.0)
        eyePos.z += (strength * kThrowZOffsetScale - kThrowZOffsetScale);

        // Trace forward to detect nearby geometry
        cs2::Vector traceEnd = eyePos + forward * kSpawnTraceForward;
        auto traceResult = traceGrenadeHull(eyePos, traceEnd, skipEntity);

        if (!traceResult.hasValue())
            return {};

        cs2::Vector hitPos;
        if (traceResult.value().fraction < 1.0f) {
            hitPos = traceResult.value().endPos;
        } else {
            hitPos = traceEnd;
        }

        // Pull back from hit position to keep grenade out of walls.
        // Clamp so spawn never goes behind eyePos.
        // Without this, standing 2u from a wall → hitPos at 2u → minus pullback = behind player.
        cs2::Vector spawnPos = hitPos - forward * kSpawnPullBack;
        cs2::Vector eyeToSpawn = spawnPos - eyePos;
        // If the dot product with forward is negative, spawn would be behind the player.
        if (eyeToSpawn.dot(forward) < 0.0f)
            spawnPos = eyePos;
        return Optional<cs2::Vector>{spawnPos};
    }

    // ClipVelocity regression response with push-off epsilon.
    // Apply only when the grenade moves into the surface (dot < 0).
    // When dot >= 0 (moving away/parallel), return velocity unchanged — the push-off
    // epsilon would otherwise inject artificial velocity every tick the grenade
    // slides parallel to a surface.
    static cs2::Vector clipVelocity(cs2::Vector velocity, cs2::Vector normal, float overbounce) noexcept
    {
        float dot = velocity.dot(normal);
        if (dot >= 0.0f)
            return velocity; // already moving away from surface, no clipping needed

        float backoff = -dot * overbounce + grenade_prediction_params::kClipPushOff;
        return cs2::Vector{
            velocity.x + normal.x * backoff,
            velocity.y + normal.y * backoff,
            velocity.z + normal.z * backoff
        };
    }

    // IDA's separate helper for optional continuation displacement.
    static cs2::Vector serverPushOff(cs2::Vector point, cs2::Vector normal, float scale) noexcept
    {
        float pushOff = (std::max)(-point.dot(normal) * scale, 0.0f) + grenade_prediction_params::kClipPushOff;
        return point + normal * pushOff;
    }

    static cs2::Vector forwardFromAngles(float pitch, float yaw) noexcept
    {
        float p = pitch * 3.14159265f / 180.0f;
        float y = yaw * 3.14159265f / 180.0f;
        float sp, cp, sy, cy;
        Math::sincos(p, sp, cp);
        Math::sincos(y, sy, cy);
        return cs2::Vector{cp * cy, cp * sy, -sp};
    }

    static float normalizeThrowStrength(float strength) noexcept
    {
        return strength > 0.4f && strength < 0.6f ? 0.5f : strength;
    }

private:
    struct CollisionResult {
        bool traceSucceeded = true;
        bool impactDetonate = false;
        bool stopped = false;
    };

    [[nodiscard]] Optional<TraceResult> traceGrenadeHull(cs2::Vector start, cs2::Vector end, void* skipEntity) noexcept
    {
        // Obtaining the provider through HookContext keeps the production trace
        // implementation unchanged while allowing a test HookContext to supply a
        // deterministic scripted provider.
        return hookContext.template make<EngineTrace>().traceGrenadeHull(start, end, skipEntity);
    }

    StepResult step(cs2::Vector& pos, cs2::Vector& vel, cs2::GrenadeKind kind, void* skipEntity) noexcept
    {
        using namespace grenade_prediction_params;

        StepResult result;
        for (int substep = 0; substep < kMovementSubsteps; ++substep) {
            const auto substepResult = movementSubstep(pos, vel, kind, skipEntity, result);
            if (!substepResult.traceSucceeded) {
                result.traceSucceeded = false;
                return result;
            }
            if (substepResult.impactDetonate) {
                result.impactDetonate = true;
                return result;
            }
        }
        return result;
    }

    CollisionResult movementSubstep(cs2::Vector& pos, cs2::Vector& vel, cs2::GrenadeKind kind, void* skipEntity, StepResult& result) noexcept
    {
        using namespace grenade_prediction_params;

        // Apply gravity using Velocity Verlet (half-step) integration.
        // Reference doc (Section 3) specifies:
        //   v_z(t+1) = v_z(t) + a_z * dt
        //   z(t+1) = z(t) + (v_z(t) + v_z(t+1)) / 2 * dt
        // This averages old and new Z velocity for position update,
        // eliminating the ~0.039 unit/tick upward bias of Explicit Euler.
        auto physics = getGrenadePhysics(kind);
        float gravity = kSvGravity * physics.gravityScale;
        float oldVelZ = vel.z;
        vel.z -= gravity * kMovementSubstepDt;
        cs2::Vector moveVec{vel.x * kMovementSubstepDt, vel.y * kMovementSubstepDt, (oldVelZ + vel.z) * 0.5f * kMovementSubstepDt};

        cs2::Vector endPos = pos + moveVec;

        auto traceResult = traceGrenadeHull(pos, endPos, skipEntity);

        if (!traceResult.hasValue())
            return {.traceSucceeded = false};

        if (traceResult.value().fraction >= 1.0f) {
            pos = endPos;
            return {};
        }

        cs2::Vector exactHitPos = traceResult.value().endPos;
        pos = exactHitPos;
        result.hit = true;
        if (result.contactsCount == 0)
            result.hitPos = exactHitPos;
        appendContact(result, exactHitPos);
        return resolvePrimaryContinuation(traceResult.value(), pos, vel, kind, skipEntity);
    }

    CollisionResult applyContactResponse(const TraceResult& trace, cs2::Vector& vel, cs2::GrenadeKind kind) noexcept
    {
        using namespace grenade_prediction_params;

        if ((kind == cs2::GrenadeKind::Molotov || kind == cs2::GrenadeKind::Incendiary) &&
            (trace.normal.z >= kMolotovSlope || vel.squareLength() < kStopSpeedSq)) {
            vel = cs2::Vector{0.0f, 0.0f, 0.0f};
            return {.impactDetonate = true, .stopped = true};
        }

        auto physics = getGrenadePhysics(kind);
        cs2::Vector newVel = clipVelocity(vel, trace.normal, 2.0f);
        newVel = newVel * physics.elasticity;

        // Entity-specific steep-bounce damping (normal.z > 0.7,
        // speed² > 96000) ONLY applies when the grenade hits a PLAYER or NPC entity,
        // not when bouncing off world geometry. Since we can't detect entity type
        // from our trace results, we skip this damping entirely — it has zero
        // effect on world-geometry bounces which are 99%+ of grenade bounces.

        // Stop check uses the configured regression value.
        if (newVel.squareLength() < kStopSpeedSq) {
            vel = cs2::Vector{0.0f, 0.0f, 0.0f};
            return {.stopped = true};
        }

        vel = newVel;
        return {};
    }

    static void appendContact(StepResult& result, cs2::Vector contact) noexcept
    {
        constexpr int kContactsCapacity = grenade_prediction_params::kMovementSubsteps;
        if (result.contactsCount < kContactsCapacity)
            result.contacts[result.contactsCount++] = contact;
    }

    CollisionResult resolvePrimaryContinuation(const TraceResult& primary, cs2::Vector& pos, cs2::Vector& vel, cs2::GrenadeKind kind, void* skipEntity) noexcept
    {
        using namespace grenade_prediction_params;

        auto collisionResult = applyContactResponse(primary, vel, kind);
        if (collisionResult.impactDetonate || collisionResult.stopped)
            return collisionResult;

        // Verified explicit server callback topology. Lower trace-provider callback re-entry
        // remains unknown and is deliberately not modeled by this explicit layer.
        cs2::Vector mandatoryMove = vel * ((1.0f - primary.fraction) * kMovementSubstepDt);
        const auto mandatory = traceGrenadeHull(pos, pos + mandatoryMove, skipEntity);
        if (!mandatory.hasValue())
            return {.traceSucceeded = false};

        if (mandatory.value().fraction >= 1.0f)
            pos = pos + mandatoryMove;
        else
            pos = mandatory.value().endPos;

        if (mandatory.value().fraction < kOptionalContinuationFractionThreshold) {
            cs2::Vector secondInput = mandatoryMove * (1.0f - mandatory.value().fraction);
            cs2::Vector secondMove = serverPushOff(secondInput, mandatory.value().normal, 2.0f);
            const auto optional = traceGrenadeHull(pos, pos + secondMove, skipEntity);
            if (!optional.hasValue())
                return {.traceSucceeded = false};

            if (optional.value().fraction >= 1.0f)
                pos = pos + secondMove;
            else
                pos = optional.value().endPos;
        }

        return {};
    }

    bool shouldDetonate(cs2::GrenadeKind kind, int tick) const noexcept
    {
        using namespace grenade_prediction_params;

        float elapsed = static_cast<float>(tick) * kSimDt;
        switch (kind) {
            case cs2::GrenadeKind::SmokeGrenade:
                // Smoke stops via displacement check in loop; time cap as safety net.
                return elapsed > kDetonateTimeSmokeCap;
            case cs2::GrenadeKind::Decoy:
                return elapsed > kDetonateTimeDecoy;
            case cs2::GrenadeKind::Molotov:
            case cs2::GrenadeKind::Incendiary:
                // Mid-air detonation: if no valid flat surface hit within time limit, explodes harmlessly.
                return static_cast<float>(tick + 1) * kSimDt > kDetonateTimeMolotov + kClientTracerHorizonPadding;
            case cs2::GrenadeKind::Flashbang:
            case cs2::GrenadeKind::HEGrenade:
                // After this step, terminate only once the client tracer horizon has been exceeded.
                return static_cast<float>(tick + 1) * kSimDt > kDetonateTimeHeFlash + kClientTracerHorizonPadding;
            default:
                return false;
        }
    }

    HookContext& hookContext;

    friend struct GrenadeSimulatorTestAccess<HookContext>;
};
