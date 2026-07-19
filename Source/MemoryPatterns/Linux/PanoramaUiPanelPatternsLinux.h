#pragma once

#include <MemoryPatterns/PatternTypes/UiPanelPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct PanoramaUiPanelPatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<SetParentFunctionOffset, CodePattern{"48 8B 90 ? ? ? ? 48 85 F6 74 16"}.add(3).read()>()
            .template addPattern<SetVisibleFunctionOffset, CodePattern{"48 8B 07 FF 90 ? ? ? ? 4C 89 F6 48 89 DF"}.add(5).read()>()
            .template addPattern<GetAttributeStringFunctionOffset, CodePattern{"FF 90 ? ? ? ? 80 BB 90 ? ? ? ? 48 89 C2"}.add(2).read()>()
            .template addPattern<SetAttributeStringFunctionOffset, CodePattern{"FF 90 ? ? ? ? 44 8B BD ? ? ? ? 45"}.add(2).read()>();
    }

    [[nodiscard]] static consteval auto addPanoramaPatterns(auto panoramaPatterns) noexcept
    {
        return panoramaPatterns
            .template addPattern<ChildPanelsVectorOffset, CodePattern{"A8 ? 0F 85 ? ? ? ? 8B ? ? 85 D2"}.add(10).read()>()
            .template addPattern<PanelClassesVectorOffset, CodePattern{"97 ? ? ? ? 85 D2 7E ? 48 8B 87"}.add(1).read()>()
            .template addPattern<PanelStyleOffset, CodePattern{"67 ? 53 48 83"}.add(1).read()>()
            .template addPattern<ParentWindowOffset, CodePattern{"? 48 85 D2 74 ? 48 89 53"}.read()>()
            .template addPattern<OffsetToPanelId, CodePattern{"50 ? ? 83 ? ? 00 0F 84 ? ? ? ? ? 8D"}.add(5).read()>()
            .template addPattern<OffsetToPanelFlags, CodePattern{"3E 41 F6 87 ? ? ? ?"}.add(4).read()>()
            .template addPattern<OffsetToPanelHandle, CodePattern{"48 8B 86 ? ? ? ? C3"}.add(3).read()>();
    }
};
