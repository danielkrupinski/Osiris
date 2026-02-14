#pragma once

#include <MemoryPatterns/PatternTypes/UiPanelPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct PanoramaUiPanelPatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<SetParentFunctionOffset, CodePattern{"48 8B 90 ? ? ? ? 48 85 F6 74 16"}.add(3).read()>()
            .template addPattern<SetVisibleFunctionOffset, CodePattern{"49 8B 84 24 ? ? ? ? 31 F6 48 8B 78 ? 48 8B 07 FF 90 ? ? ? ? 4C 89 F7 E8"}.add(19).read()>()
            .template addPattern<GetAttributeStringFunctionOffset, CodePattern{"FF 90 ? ? ? ? 80 BB 90 ? ? ? ? 48 89 C2"}.add(2).read()>()
            .template addPattern<SetAttributeStringFunctionOffset, CodePattern{"78 ? 48 8B 07 FF 90 ? ? ? ? 44 8B 8D"}.add(7).read()>();
    }

    [[nodiscard]] static consteval auto addPanoramaPatterns(auto panoramaPatterns) noexcept
    {
        return panoramaPatterns
            .template addPattern<ChildPanelsVectorOffset, CodePattern{"A8 ? 0F 85 ? ? ? ? 8B ? ? 85 D2"}.add(10).read()>()
            .template addPattern<PanelClassesVectorOffset, CodePattern{"97 ? ? ? ? 85 D2 7E ? 48 8B 87"}.add(1).read()>()
            .template addPattern<PanelStyleOffset, CodePattern{"E8 ? ? ? ? 48 8D 43 ? 48 8B"}.add(8).read()>()
            .template addPattern<ParentWindowOffset, CodePattern{"? 48 85 D2 74 ? 48 89 53"}.read()>()
            .template addPattern<OffsetToPanelId, CodePattern{"E7 E8 ? ? ? ? 48 83 7B ? 00 0F"}.add(9).read()>()
            .template addPattern<OffsetToPanelFlags, CodePattern{"3E 41 F6 87 ? ? ? ?"}.add(4).read()>()
            .template addPattern<OffsetToPanelHandle, CodePattern{"48 8B 86 ? ? ? ? C3"}.add(3).read()>();
    }
};
