#pragma once

#include <CS2/Panorama/CPanelStyle.h>
#include <MemoryPatterns/PatternTypes/PanelStylePatternTypes.h>
#include <MemorySearch/BytePatternLiteral.h>
#include <MemorySearch/CodePattern.h>

struct PanelStylePatterns2 {
    [[nodiscard]] static consteval auto addPanoramaPatterns(auto panoramaPatterns) noexcept
    {
        return panoramaPatterns
            .template addPattern<SetPanelStylePropertyFunctionPointer, CodePattern{"55 48 89 E5 41 57 49 89 F7 41 56 41 55 41 89"}>();
    }
};

template <typename PatternFinders>
struct PanelStylePatterns {
    const PatternFinders& patternFinders;

    [[nodiscard]] cs2::CPanelStyle::StylePropertySymbols* stylePropertiesSymbols() const noexcept
    {
        return patternFinders.panoramaPatternFinder("DE 48 C7 05 ? ? ? ? 00 00 00 00 48 8D"_pat).add(4).abs(8).template as<cs2::CPanelStyle::StylePropertySymbols*>();
    }
};
