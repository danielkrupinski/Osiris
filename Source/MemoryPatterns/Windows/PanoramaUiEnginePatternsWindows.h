#pragma once

#include <MemoryPatterns/PatternTypes/UiEnginePatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct PanoramaUiEnginePatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<UiEnginePointer, CodePattern{"48 89 78 ? 48 89 0D ? ? ? ?"}.add(7).abs()>();
    }

    [[nodiscard]] static consteval auto addPanoramaPatterns(auto panoramaPatterns) noexcept
    {
        return panoramaPatterns
            .template addPattern<GetPanelPointerFunctionPointer, CodePattern{"4C 63 0A 4C 8B DA"}>()
            .template addPattern<RunScriptFunctionPointer, CodePattern{"48 89 5C 24 10 4C 89 4C 24 20 4C 89 44 24 18 55 56 57 41 54 41 55 41 56 41 57 48 8D 6C 24 80 48 81 EC 80 01 00 00"}>()
            .template addPattern<MakeSymbolFunctionPointer, CodePattern{"40 55 56 48 83 EC 48 48 63 EA 49 8B F0 4D 85 C0 0F 84 ? ? ? ? 41 80 38"}>()
            .template addPattern<OnDeletePanelFunctionPointer, CodePattern{"48 85 D2 0F 84 ? ? ? ? 48 89 ? 24 ? 57 48 83 EC ? 48"}>()
            .template addPattern<RegisterEventHandlerFunctionPointer, CodePattern{"48 89 5C 24 ? 66 89 54 24 ? 55 56 57 41 56 41 57 48 83 EC ? 48 8D 05 ? ? ? ? 48 C7 44 24 28 ? ? ? ? 48 89 44 24 ? 4D"}>();
    }
};
