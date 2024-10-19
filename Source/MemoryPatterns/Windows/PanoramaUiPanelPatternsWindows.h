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
            .template addPattern<SetParentFunctionOffset, CodePattern{"48 8B B8 ? ? ? ? 48 85 D2"}.add(3).read()>()
            .template addPattern<SetVisibleFunctionOffset, CodePattern{"01 FF 90 ? ? ? ? 45 33 C0 33 D2 ? 8B ? E8"}.add(3).read()>()
            .template addPattern<GetAttributeStringFunctionOffset, CodePattern{"12 48 8B 01 FF 90 ? ? ? ? 48 8B ? 48 85 C0 74 ? 80 38 00 74 ? 48"}.add(6).read()>()
            .template addPattern<SetAttributeStringFunctionOffset, CodePattern{"FF 90 ? ? ? ? 48 83 C6 ? 48 3B ? 75 ? 4C"}.add(2).read()>();
    }
};

template <typename PatternFinders>
struct PanoramaUiPanelPatterns {
    const PatternFinders& patternFinders;

    [[nodiscard]] ChildPanelsVectorOffset childPanelsVectorOffset() const noexcept
    {
        return patternFinders.panoramaPatternFinder("83 ? ? 00 0F 84 ? ? ? ? 48 8B ? ? 0C ? 88"_pat).add(2).template readOffset<ChildPanelsVectorOffset>();
    }

    [[nodiscard]] PanelClassesVectorOffset classesVectorOffset() const noexcept
    {
        return patternFinders.panoramaPatternFinder("48 63 88 ? ? ? ? 45 33 C0"_pat).add(3).template readOffset<PanelClassesVectorOffset>();
    }

    [[nodiscard]] PanelStyleOffset panelStyleOffset() const noexcept
    {
        return patternFinders.panoramaPatternFinder("D2 E8 ? ? ? ? 48 8D 43 ? 48"_pat).add(9).template readOffset<PanelStyleOffset>();
    }

    [[nodiscard]] ParentWindowOffset parentWindowOffset() const noexcept
    {
        return patternFinders.panoramaPatternFinder("48 89 ? ? 48 85 ? 75 ? 48 85"_pat).add(3).template readOffset<ParentWindowOffset>();
    }

    [[nodiscard]] OffsetToPanelId offsetToPanelId() const noexcept
    {
        return patternFinders.panoramaPatternFinder("? 48 8B 6C 24 ? 74 ? FF 15 ? ? ? ? 48 8D"_pat).template readOffset<OffsetToPanelId>();
    }

    [[nodiscard]] OffsetToPanelFlags offsetToPanelFlags() const noexcept
    {
        return patternFinders.panoramaPatternFinder("06 48 8B 0C 07 F6 81 ? ? ? ?"_pat).add(7).template readOffset<OffsetToPanelFlags>();
    }
};
