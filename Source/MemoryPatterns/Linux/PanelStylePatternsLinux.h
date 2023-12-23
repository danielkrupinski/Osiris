#pragma once

#include <MemoryPatterns/PanelStylePatterns.h>
#include <MemorySearch/BytePatternLiteral.h>

inline cs2::CPanelStyle::SetProperty* PanelStylePatterns::setProperty() const noexcept
{
    return panoramaPatternFinder("E9 ? ? ? ? 0F 1F 00 48 8B 78 08"_pat).add(1).abs().as<cs2::CPanelStyle::SetProperty*>();
}

inline cs2::CPanelStyle::StylePropertySymbols* PanelStylePatterns::stylePropertiesSymbols() const noexcept
{
    return panoramaPatternFinder("E8 ? ? ? ? 4C 89 FE 4C 89 F7 48 C7"_pat).add(1).abs().add(14).abs().as<cs2::CPanelStyle::StylePropertySymbols*>();
}
