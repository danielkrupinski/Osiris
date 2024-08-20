#pragma once

#include <CS2/Panorama/CPanelStyle.h>
#include <MemorySearch/BytePatternLiteral.h>

template <typename PatternFinders>
struct PanelStylePatterns {
    const PatternFinders& patternFinders;

    [[nodiscard]] cs2::CPanelStyle::SetProperty* setProperty() const noexcept
    {
        return patternFinders.panoramaPatternFinder("E9 ? ? ? ? 0F 1F 00 48 8B 78 08"_pat).add(1).abs().template as<cs2::CPanelStyle::SetProperty*>();
    }

    [[nodiscard]] cs2::CPanelStyle::StylePropertySymbols* stylePropertiesSymbols() const noexcept
    {
        return patternFinders.panoramaPatternFinder.matchPatternAtAddress(patternFinders.panoramaPatternFinder("48 89 75 ? E8 ? ? ? ? ? 89 ? 4C 89 ? 48 C7"_pat).add(5).abs().template as<void*>(), "0F B6 05 ? ? ? ? 84 C0 74 0D 48 8D 05 ? ? ? ?"_pat).add(14).abs().template as<cs2::CPanelStyle::StylePropertySymbols*>();
    }
};
