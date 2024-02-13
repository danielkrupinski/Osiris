#pragma once

#include <MemoryPatterns/PanelStylePatterns.h>
#include <MemorySearch/BytePatternLiteral.h>

inline cs2::CPanelStyle::SetProperty* PanelStylePatterns::setProperty() const noexcept
{
    return panoramaPatternFinder("E9 ? ? ? ? 0F 1F 00 48 8B 78 08"_pat).add(1).abs().as<cs2::CPanelStyle::SetProperty*>();
}

inline cs2::CPanelStyle::StylePropertySymbols* PanelStylePatterns::stylePropertiesSymbols() const noexcept
{
    return panoramaPatternFinder.matchPatternAtAddress(panoramaPatternFinder("48 89 75 ? E8 ? ? ? ? ? 89 ? 4C 89 ? 48 C7"_pat).add(5).abs().as<void*>(), "0F B6 05 ? ? ? ? 84 C0 74 0D 48 8D 05 ? ? ? ?"_pat).add(14).abs().as<cs2::CPanelStyle::StylePropertySymbols*>();
}
