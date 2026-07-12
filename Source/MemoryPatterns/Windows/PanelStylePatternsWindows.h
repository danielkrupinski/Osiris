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
            .template addPattern<SetPanelStylePropertyFunctionPointer, CodePattern{"E8 ? ? ? ? 48 8D 05 ? ? ? ? 48 89 45 ? EB ? 0F"}.add(1).abs()>();
    }
};

template <typename PatternFinders>
struct PanelStylePatterns {
    const PatternFinders& patternFinders;

    [[nodiscard]] cs2::CPanelStyle::StylePropertySymbols* stylePropertiesSymbols() const noexcept
    {
        if (const auto pointerToStylePropertySymbolsMemory{patternFinders.panoramaPatternFinder("48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? C7 05 ? ? ? ? FF FF FF FF C7 05 ? ? ? ? 00 00 00 00 C7 05 ? ? ? ? FF FF FF FF"_pat).add(3).abs().template as<std::byte*>()})
            return reinterpret_cast<cs2::CPanelStyle::StylePropertySymbols*>(pointerToStylePropertySymbolsMemory);
        return nullptr;
    }
};
