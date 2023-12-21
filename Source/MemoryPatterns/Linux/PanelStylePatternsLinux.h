#pragma once

#include <GlobalContext.h>
#include <MemoryPatterns/PanelStylePatterns.h>
#include <MemorySearch/BytePatternLiteral.h>

inline cs2::CPanelStyle::SetProperty* PanelStylePatterns::setProperty() noexcept
{
    return GlobalContext::instance().panoramaPatternFinder("E9 ? ? ? ? 0F 1F 00 48 8B 78 08"_pat).add(1).abs().as<cs2::CPanelStyle::SetProperty*>();
}

inline cs2::CPanelStyle::StylePropertySymbols* PanelStylePatterns::stylePropertiesSymbols() noexcept
{
    return GlobalContext::instance().panoramaPatternFinder("E8 ? ? ? ? 4C 89 FE 4C 89 F7 48 C7"_pat).add(1).abs().add(14).abs().as<cs2::CPanelStyle::StylePropertySymbols*>();
}
