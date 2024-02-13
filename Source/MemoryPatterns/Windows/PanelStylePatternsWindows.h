#pragma once

#include <cstddef>

#include <MemoryPatterns/PanelStylePatterns.h>
#include <MemorySearch/BytePatternLiteral.h>

inline cs2::CPanelStyle::SetProperty* PanelStylePatterns::setProperty() const noexcept
{
    return panoramaPatternFinder("E8 ? ? ? ? 8B 45 B8"_pat).add(1).abs().as<cs2::CPanelStyle::SetProperty*>();
}

inline cs2::CPanelStyle::StylePropertySymbols* PanelStylePatterns::stylePropertiesSymbols() const noexcept
{
    if (const auto pointerToStylePropertySymbolsMemory{panoramaPatternFinder("48 8B 05 ? ? ? ? 48 63 FB"_pat).add(3).abs().as<std::byte*>()})
        return reinterpret_cast<cs2::CPanelStyle::StylePropertySymbols*>(pointerToStylePropertySymbolsMemory - offsetof(cs2::CPanelStyle::StylePropertySymbols, memory));
    return nullptr;
}
