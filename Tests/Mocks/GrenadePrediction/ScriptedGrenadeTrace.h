#pragma once

#include <Platform/Macros/IsPlatform.h>
#include <Features/Visuals/GrenadePrediction/GrenadeSimulator.h>

struct ScriptedGrenadeTrace {
    static constexpr int kMaxResults = 1200;

    Optional<TraceResult> results[kMaxResults]{};
    int resultCount{};
    int calls{};
    Optional<TraceResult> fallback{};

    void push(Optional<TraceResult> result) noexcept
    {
        if (resultCount < kMaxResults)
            results[resultCount++] = result;
    }

    void clearAfterScript() noexcept
    {
        fallback = TraceResult{1.0f, {}, {}};
    }

    [[nodiscard]] Optional<TraceResult> traceGrenadeHull(cs2::Vector, cs2::Vector, void*) noexcept
    {
        if (calls < resultCount)
            return results[calls++];
        ++calls;
        return fallback;
    }
};

struct GrenadeSimulatorTestHookContext {
    ScriptedGrenadeTrace trace;

    template <template <typename> typename T>
    [[nodiscard]] ScriptedGrenadeTrace& make() noexcept
    {
        return trace;
    }
};

template <typename HookContext>
struct GrenadeSimulatorTestAccess {
    using Simulator = GrenadeSimulator<HookContext>;

    [[nodiscard]] static StepResult step(Simulator& simulator, cs2::Vector& pos, cs2::Vector& velocity, cs2::GrenadeKind kind, void* skipEntity = nullptr, float friction = 0.0f) noexcept
    {
        return simulator.step(pos, velocity, kind, skipEntity, friction);
    }

    [[nodiscard]] static bool shouldDetonate(const Simulator& simulator, cs2::GrenadeKind kind, int tick) noexcept
    {
        return simulator.shouldDetonate(kind, tick);
    }
};
