#pragma once

#include <cstddef>

#include <CS2/Panorama/CPanelStyle.h>
#include <MemoryPatterns/PatternTypes/PanelStylePatternTypes.h>
#include <MemorySearch/BytePatternLiteral.h>
#include <MemorySearch/CodePattern.h>

struct PanelStylePatterns2 {
    [[nodiscard]] static consteval auto addPanoramaPatterns(auto panoramaPatterns) noexcept
    {
        return panoramaPatterns
            .template addPattern<SetPanelStylePropertyFunctionPointer, CodePattern{"E8 ? ? ? ? 8B 45 B8"}.add(1).abs()>();
    }
};

template <typename PatternFinders>
struct PanelStylePatterns {
    const PatternFinders& patternFinders;

    [[nodiscard]] cs2::CPanelStyle::StylePropertySymbols* stylePropertiesSymbols() const noexcept
    {
        if (const auto pointerToStylePropertySymbolsMemory{patternFinders.panoramaPatternFinder("48 8B 05 ? ? ? ? 48 63 FB"_pat).add(3).abs().template as<std::byte*>()})
            return reinterpret_cast<cs2::CPanelStyle::StylePropertySymbols*>(pointerToStylePropertySymbolsMemory - offsetof(cs2::CPanelStyle::StylePropertySymbols, memory));
        return nullptr;
    }
};
