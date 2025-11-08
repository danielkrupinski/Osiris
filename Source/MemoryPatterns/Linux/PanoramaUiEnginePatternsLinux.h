#pragma once

#include <MemoryPatterns/PatternTypes/UiEnginePatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct PanoramaUiEnginePatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<UiEnginePointer, CodePattern{"48 89 3D ? ? ? ? E8 ? ? ? ? 48 8B 3D ? ? ? ? E8 ? ? ? ? 48"}.add(3).abs()>();
    }

    [[nodiscard]] static consteval auto addPanoramaPatterns(auto panoramaPatterns) noexcept
    {
        return panoramaPatterns
            .template addPattern<GetPanelPointerFunctionPointer, CodePattern{"8B 16 31 C9"}>()
            .template addPattern<RunScriptFunctionPointer, CodePattern{"55 48 8D 05 ? ? ? ? 48 89 E5 41 57 49 89 D7 41 56 49"}>()
            .template addPattern<MakeSymbolFunctionPointer, CodePattern{"55 48 89 E5 41 57 41 56 41 55 41 54 53 48 83 EC ? 48 85 D2"}>()
            .template addPattern<OnDeletePanelFunctionPointer, CodePattern{"48 85 F6 0F 84 ? ? ? ? 55 48 89 E5 41 56 41 55 41 54 49 89 F4 53 48 8B 06 48 89 FB 48 89 F7 FF 10"}>()
            .template addPattern<RegisterEventHandlerFunctionPointer, CodePattern{"55 48 8D 05 ? ? ? ? 48 89 E5 41 57 41 56 49 89 FE 41 55 48 8D 3D ? ? ? ? 49 89 CD"}>();
    }
};
