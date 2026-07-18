#pragma once

#include <Platform/Macros/IsPlatform.h>
#include <Features/Visuals/GrenadePrediction/GrenadeSimulator.h>

struct ScriptedGrenadeTrace {
    static constexpr int kMaxResults = 1200;
    static constexpr int kMaxCallHistory = 16;

    Optional<TraceResult> results[kMaxResults]{};
    int resultCount{};
    int calls{};
    cs2::Vector starts[kMaxCallHistory]{};
    cs2::Vector ends[kMaxCallHistory]{};
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

    [[nodiscard]] Optional<TraceResult> traceGrenadeHull(cs2::Vector start, cs2::Vector end, void*) noexcept
    {
        if (calls < kMaxCallHistory) {
            starts[calls] = start;
            ends[calls] = end;
        }
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

    [[nodiscard]] static StepResult step(Simulator& simulator, cs2::Vector& pos, cs2::Vector& velocity, cs2::GrenadeKind kind, void* skipEntity = nullptr) noexcept
    {
        return simulator.step(pos, velocity, kind, skipEntity);
    }

    [[nodiscard]] static bool shouldDetonate(const Simulator& simulator, cs2::GrenadeKind kind, int tick) noexcept
    {
        return simulator.shouldDetonate(kind, tick);
    }

    [[nodiscard]] static auto continuePrimary(Simulator& simulator, const TraceResult& primary, cs2::Vector& pos, cs2::Vector& velocity, cs2::GrenadeKind kind, void* skipEntity = nullptr) noexcept
    {
        return simulator.resolvePrimaryContinuation(primary, pos, velocity, kind, skipEntity);
    }
};
