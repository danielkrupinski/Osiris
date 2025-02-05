#pragma once

#include <MemoryPatterns/PatternTypes/UiPanelPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct PanoramaUiPanelPatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<SetParentFunctionOffset, CodePattern{"48 8B 90 ? ? ? ? 48 85 F6 74 16"}.add(3).read()>()
            .template addPattern<SetVisibleFunctionOffset, CodePattern{"F6 48 8B 78 ? 48 8B 07 FF 90 ? ? ? ? E9 ? ? ? ? FF"}.add(10).read()>()
            .template addPattern<GetAttributeStringFunctionOffset, CodePattern{"FF 90 ? ? ? ? 41 80 BC 24 ? ? ? ? ? 48 89 C2"}.add(2).read()>()
            .template addPattern<SetAttributeStringFunctionOffset, CodePattern{"FF 90 ? ? ? ? 8B BD ? ? ? ? 85"}.add(2).read()>();
    }

    [[nodiscard]] static consteval auto addPanoramaPatterns(auto panoramaPatterns) noexcept
    {
        return panoramaPatterns
            .template addPattern<ChildPanelsVectorOffset, CodePattern{"0F 85 ? ? ? ? 8B ? ? 85 D2 0F 84"}.add(8).read()>()
            .template addPattern<PanelClassesVectorOffset, CodePattern{"74 33 8B 97 ? ? ? ? 85"}.add(4).read()>()
            .template addPattern<PanelStyleOffset, CodePattern{"E8 ? ? ? ? 48 8D 43 ? 48 8B"}.add(8).read()>()
            .template addPattern<ParentWindowOffset, CodePattern{"4D 89 ? 24 ? 4D 85 ? 0F 84 ? ? ? ? 4C"}.add(4).read()>()
            .template addPattern<OffsetToPanelId, CodePattern{"00 48 83 7B ? 00 74 ? 48 8D 7B ? 5B 41 5C 41"}.add(4).read()>()
            .template addPattern<OffsetToPanelFlags, CodePattern{"D8 41 F6 85 ? ? ? ?"}.add(4).read()>();
    }
};
