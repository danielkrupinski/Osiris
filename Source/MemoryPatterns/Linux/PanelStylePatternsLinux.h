#pragma once

#include <GlobalContext.h>
#include <MemoryPatterns/PanelStylePatterns.h>
#include <MemorySearch/BytePatternLiteral.h>

inline cs2::CPanelStyle::SetProperty* PanelStylePatterns::setProperty() noexcept
{
    return GlobalContext::instance().panoramaPatternFinder("E9 ? ? ? ? 0F 1F 00 48 8B 78 08"_pat).add(1).abs().as<cs2::CPanelStyle::SetProperty*>();
}
