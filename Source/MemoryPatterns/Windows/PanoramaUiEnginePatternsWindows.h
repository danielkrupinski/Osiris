#pragma once

#include <MemoryPatterns/PatternTypes/UiPanelPatternTypes.h>
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
            .template addPattern<GetPanelHandleFunctionPointer, CodePattern{"8B 81 ? ? ? ? 4C 8B D9 83 F8 FF 74 41"}>()
            .template addPattern<GetPanelPointerFunctionPointer, CodePattern{"4C 63 02 4D 8B C8"}>()
            .template addPattern<RunScriptFunctionPointer, CodePattern{"4C 89 4C 24 ? 4C 89 44 24 ? 48 89 54 24 ? 55"}>()
            .template addPattern<MakeSymbolFunctionPointer, CodePattern{"C9 E8 ? ? ? ? 4C 63 2E"}.add(2).abs()>()
            .template addPattern<OnDeletePanelFunctionPointer, CodePattern{"48 89 5C 24 ? ? 48 83 EC 20 48 8B DA 48 8B F9 48 85 D2 0F 84"}>();
    }
};
