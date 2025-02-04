#pragma once

#include <cstdint>

#include <MemoryPatterns/PatternTypes/UiPanelPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct PanoramaUiPanelPatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<SetParentFunctionOffset, CodePattern{"48 8B B8 ? ? ? ? 48 85 D2"}.add(3).read()>()
            .template addPattern<SetVisibleFunctionOffset, CodePattern{"01 FF 90 ? ? ? ? 45 33 C0 33 D2 ? 8B ? E8"}.add(3).read()>()
            .template addPattern<GetAttributeStringFunctionOffset, CodePattern{"12 48 8B 01 FF 90 ? ? ? ? 48 8B ? 48 85 C0 74 ? 80 38 00 74 ? 48 8D 4C"}.add(6).read()>()
            .template addPattern<SetAttributeStringFunctionOffset, CodePattern{"FF 90 ? ? ? ? 48 83 C6 ? 48 3B ? 75 ? 4C"}.add(2).read()>();
    }

    [[nodiscard]] static consteval auto addPanoramaPatterns(auto panoramaPatterns) noexcept
    {
        return panoramaPatterns
            .template addPattern<ChildPanelsVectorOffset, CodePattern{"83 ? ? 00 0F 84 ? ? ? ? 48 8B ? ? 0C ? 88"}.add(2).read()>()
            .template addPattern<PanelClassesVectorOffset, CodePattern{"48 63 88 ? ? ? ? 45 33 C0"}.add(3).read()>()
            .template addPattern<PanelStyleOffset, CodePattern{"D2 E8 ? ? ? ? 48 8D 43 ? 48"}.add(9).read()>()
            .template addPattern<ParentWindowOffset, CodePattern{"48 89 ? ? 48 85 ? 75 ? 48 85"}.add(3).read()>()
            .template addPattern<OffsetToPanelId, CodePattern{"? 48 8B 6C 24 ? 74 ? FF 15 ? ? ? ? 48 8D"}.read()>()
            .template addPattern<OffsetToPanelFlags, CodePattern{"06 48 8B 0C 07 F6 81 ? ? ? ?"}.add(7).read()>();
    }
};
