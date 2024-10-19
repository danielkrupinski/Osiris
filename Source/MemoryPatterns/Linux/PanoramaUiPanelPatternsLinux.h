#pragma once

#include <cstdint>

#include <MemoryPatterns/PatternTypes/UiPanelPatternTypes.h>
#include <MemorySearch/CodePattern.h>

#include <GameClasses/OffsetTypes/PanoramaUiPanelOffset.h>
#include <MemorySearch/BytePatternLiteral.h>

struct PanoramaUiPanelPatterns2 {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<SetParentFunctionOffset, CodePattern{"48 8B 90 ? ? ? ? 48 85 F6 74 16"}.add(3).read()>()
            .template addPattern<SetVisibleFunctionOffset, CodePattern{"F6 48 8B 78 ? 48 8B 07 FF 90 ? ? ? ? E9 ? ? ? ? ? 8D"}.add(10).read()>()
            .template addPattern<GetAttributeStringFunctionOffset, CodePattern{"FF 90 ? ? ? ? 41 80 BC 24 ? ? ? ? ? 48 89 C2"}.add(2).read()>()
            .template addPattern<SetAttributeStringFunctionOffset, CodePattern{"FF 90 ? ? ? ? 8B 05 ? ? ? ? 39 85 ? ? ? ? 0F 84 ? ? ? ? 48 8B 3B"}.add(2).read()>();
    }
};

template <typename PatternFinders>
struct PanoramaUiPanelPatterns {
    const PatternFinders& patternFinders;

    [[nodiscard]] ChildPanelsVectorOffset childPanelsVectorOffset() const noexcept
    {
        return patternFinders.panoramaPatternFinder("0F 85 ? ? ? ? 8B ? ? 85 D2 0F 84"_pat).add(8).template readOffset<ChildPanelsVectorOffset>();
    }

    [[nodiscard]] PanelClassesVectorOffset classesVectorOffset() const noexcept
    {
        return patternFinders.panoramaPatternFinder("74 33 8B 97 ? ? ? ? 85"_pat).add(4).template readOffset<PanelClassesVectorOffset>();
    }

    [[nodiscard]] PanelStyleOffset panelStyleOffset() const noexcept
    {
        return patternFinders.panoramaPatternFinder("E8 ? ? ? ? 48 8D 43 ? 48 8B"_pat).add(8).template readOffset<PanelStyleOffset>();
    }

    [[nodiscard]] ParentWindowOffset parentWindowOffset() const noexcept
    {
        return patternFinders.panoramaPatternFinder("4D 89 ? 24 ? 4D 85 ? 0F 84 ? ? ? ? 4C"_pat).add(4).template readOffset<ParentWindowOffset>();
    }
    
    [[nodiscard]] OffsetToPanelId offsetToPanelId() const noexcept
    {
        return patternFinders.panoramaPatternFinder("00 48 83 7B ? 00 74 ? 48 8D 7B ? 5B 41 5C 41"_pat).add(4).template readOffset<OffsetToPanelId>();
    }

    [[nodiscard]] OffsetToPanelFlags offsetToPanelFlags() const noexcept
    {
        return patternFinders.panoramaPatternFinder("D8 41 F6 85 ? ? ? ?"_pat).add(4).template readOffset<OffsetToPanelFlags>();
    }
};
