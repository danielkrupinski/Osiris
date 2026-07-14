#pragma once

#include <array>
#include <bit>
#include <cstdint>

#include <GameClient/EngineTrace/EngineTraceTypes.h>
#include <MemoryPatterns/MemoryPatterns.h>
#include <Utils/Optional.h>

struct TraceResult {
    float fraction;
    cs2::Vector endPos;
    cs2::Vector normal;
};

constexpr std::uint64_t CONTENTS_SOLID = 0x1;
constexpr std::uint64_t CONTENTS_HITBOXES = 0x2;
constexpr std::uint64_t CONTENTS_SKY = 0x8;
constexpr std::uint64_t CONTENTS_WINDOW = 0x1000;
constexpr std::uint64_t CONTENTS_PASSBULLETS = 0x2000;
constexpr std::uint64_t CONTENTS_PLAYER = 0x40000;
constexpr std::uint64_t CONTENTS_NPC = 0x80000;
constexpr std::uint64_t CONTENTS_DEBRIS = 0x100000;
constexpr std::uint64_t MASK_SHOT = CONTENTS_SOLID | CONTENTS_HITBOXES | CONTENTS_WINDOW | CONTENTS_PASSBULLETS | CONTENTS_PLAYER | CONTENTS_NPC | CONTENTS_DEBRIS;
constexpr std::uint64_t MASK_GRENADE = (MASK_SHOT & ~CONTENTS_WINDOW) | CONTENTS_SKY;

template <typename HookContext>
class EngineTrace {
public:
    explicit EngineTrace(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    [[nodiscard]] Optional<TraceResult> traceGrenadeHull(cs2::Vector start, cs2::Vector end, void* skipEntity) const noexcept
    {
#if defined(_WIN64)
        const auto& results = hookContext.patternSearchResults();
        const auto shapeBuilder = results.template get<ShapeBuilderFunctionPointer>();
        const auto traceShape = results.template get<TraceShapeFunctionPointer>();
        const auto managerStorage = results.template get<GameTraceManagerStoragePointer>();
        const auto initFilter = results.template get<InitFilterFunctionPointer>();
        const auto endPositionOffset = results.template get<CGameTraceEndPositionOffset>();
        const auto normalOffset = results.template get<CGameTraceNormalOffset>();
        const auto fractionOffset = results.template get<CGameTraceFractionOffset>();

        if (shapeBuilder == nullptr || traceShape == nullptr || managerStorage == nullptr || initFilter == nullptr
            || !engine_trace::areValidOutputOffsets(endPositionOffset.value(), normalOffset.value(), fractionOffset.value()))
            return {};

        void* const managerHolder = *managerStorage;
        if (managerHolder == nullptr)
            return {};

        engine_trace::DescriptorStorage descriptor{};
        engine_trace::FilterStorage filter{};
        engine_trace::OutputStorage output{};
        const engine_trace::Bounds6f bounds{{-2.0f, -2.0f, -2.0f}, {2.0f, 2.0f, 2.0f}};
        shapeBuilder(&descriptor, &bounds);
        initFilter(&filter, skipEntity, MASK_GRENADE, 4, 7);
        traceShape(managerHolder, &descriptor, &start, &end, &filter, &output);

        const auto fraction = readOutput<float>(output, fractionOffset.value());
        const auto endPosition = readOutput<cs2::Vector>(output, endPositionOffset.value());
        const auto normal = readOutput<cs2::Vector>(output, normalOffset.value());
        if (!engine_trace::isFinite(fraction) || fraction < 0.0f || fraction > 1.0f || !engine_trace::isFinite(endPosition) || !engine_trace::isFinite(normal))
            return {};
        if (fraction < 1.0f && !engine_trace::hasUsableNormal(normal))
            return {};
        return TraceResult{fraction, endPosition, normal};
#else
        return {};
#endif
    }

private:
    template <typename T>
    [[nodiscard]] static T readOutput(const engine_trace::OutputStorage& output, std::int32_t offset) noexcept
    {
        std::array<std::byte, sizeof(T)> bytes{};
        for (std::size_t i = 0; i < sizeof(T); ++i)
            bytes[i] = output.storage[static_cast<std::size_t>(offset) + i];
        return std::bit_cast<T>(bytes);
    }

    HookContext& hookContext;
};
