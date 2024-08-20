#pragma once

#include <cstddef>

#include <CS2/Panorama/CPanelStyle.h>
#include <MemorySearch/BytePatternLiteral.h>

template <typename PatternFinders>
struct PanelStylePatterns {
    const PatternFinders& patternFinders;

    [[nodiscard]] cs2::CPanelStyle::SetProperty* setProperty() const noexcept
    {
        return patternFinders.panoramaPatternFinder("E8 ? ? ? ? 8B 45 B8"_pat).add(1).abs().template as<cs2::CPanelStyle::SetProperty*>();
    }

    [[nodiscard]] cs2::CPanelStyle::StylePropertySymbols* stylePropertiesSymbols() const noexcept
    {
        if (const auto pointerToStylePropertySymbolsMemory{patternFinders.panoramaPatternFinder("48 8B 05 ? ? ? ? 48 63 FB"_pat).add(3).abs().template as<std::byte*>()})
            return reinterpret_cast<cs2::CPanelStyle::StylePropertySymbols*>(pointerToStylePropertySymbolsMemory - offsetof(cs2::CPanelStyle::StylePropertySymbols, memory));
        return nullptr;
    }
};
