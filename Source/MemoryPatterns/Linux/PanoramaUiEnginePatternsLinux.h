#pragma once

#include <MemoryPatterns/PatternTypes/UiEnginePatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct PanoramaUiEnginePatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<UiEnginePointer, CodePattern{"48 89 E5 48 89 3D ? ? ? ? E8"}.add(6).abs()>();
    }

    [[nodiscard]] static consteval auto addPanoramaPatterns(auto panoramaPatterns) noexcept
    {
        return panoramaPatterns
            .template addPattern<GetPanelHandleFunctionPointer, CodePattern{"8B 97 ? ? ? ? 83 FA FF 74 2E"}>()
            .template addPattern<GetPanelPointerFunctionPointer, CodePattern{"8B 06 45 31 C0 85 C0"}>()
            .template addPattern<RunScriptFunctionPointer, CodePattern{"55 48 89 E5 41 57 49 89 CF 41 56 49 89 F6 41 55 41"}>()
            .template addPattern<MakeSymbolFunctionPointer, CodePattern{"89 F7 48 89 D6 E9 ? ? ? ? 66 0F 1F 44 00 00 89 F7 48 89 D6"}>()
            .template addPattern<OnDeletePanelFunctionPointer, CodePattern{"55 48 89 E5 41 54 49 89 FC 53 48 89 F3 48 83 EC 10"}>();
    }
};
