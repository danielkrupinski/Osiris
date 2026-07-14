#pragma once

#include <MemoryPatterns/PatternTypes/EngineTracePatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct EngineTracePatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<ShapeBuilderFunctionPointer, CodePattern{"F3 0F 10 42 0C 0F 2E 02 7A 36 75 34 F3 0F 10 42 10 0F 2E 42 04 7A 29 75 27"}>()
            .template addPattern<TraceShapeFunctionPointer, CodePattern{"0F 97 45 ? E8 ? ? ? ? 48 8D 55"}.add(5).abs()>()
            .template addPattern<GameTraceManagerStoragePointer, CodePattern{"48 8B 0D ? ? ? ? 4C 8D 4B ? 4C 8D 43"}.add(3).abs()>()
            .template addPattern<InitFilterFunctionPointer, CodePattern{"48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 20 0F B6 41 ? 33 FF 24 C9"}>()
            .template addPattern<CGameTraceEndPositionOffset, CodePattern{"F2 0F 11 83 ? ? ? ? F3 0F 11 93 ? ? ? ? 48 8B 9C 24 ? ? ? ?"}.add(4).read()>()
            .template addPattern<CGameTraceNormalOffset, CodePattern{"F2 41 0F 10 46 1C F2 0F 11 83 ? ? ? ? 41 8B 46 24 89 83 ? ? ? ?"}.add(10).read()>()
            .template addPattern<CGameTraceFractionOffset, CodePattern{"41 8B 46 34 89 83 ? ? ? ? 41 0F B6 46 40 88 83 ? ? ? ?"}.add(6).read()>();
    }
};
