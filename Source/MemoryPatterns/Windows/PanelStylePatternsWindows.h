#pragma once

#include <GlobalContext.h>
#include <MemoryPatterns/PanelStylePatterns.h>
#include <MemorySearch/BytePatternLiteral.h>

inline cs2::CPanelStyle::SetProperty* PanelStylePatterns::setProperty() noexcept
{
    return GlobalContext::instance().panoramaPatternFinder("E8 ? ? ? ? 8B 45 B8"_pat).add(1).abs().as<cs2::CPanelStyle::SetProperty*>();
}
