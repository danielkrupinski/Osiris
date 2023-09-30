#pragma once

#include <GlobalContext.h>
#include <MemoryPatterns/PanoramaUiEnginePatterns.h>
#include <MemorySearch/BytePatternLiteral.h>

inline cs2::CUIEngine::getPanelHandle PanoramaUiEnginePatterns::getPanelHandle() noexcept
{
    return GlobalContext::instance().panoramaPatternFinder("8B 97 ? ? ? ? 83 FA FF 74 2E"_pat).as<cs2::CUIEngine::getPanelHandle>();
}

inline cs2::CUIEngine::getPanelPointer PanoramaUiEnginePatterns::getPanelPointer() noexcept
{
    return GlobalContext::instance().panoramaPatternFinder("8B 06 45 31 C0 85 C0"_pat).as<cs2::CUIEngine::getPanelPointer>();
}

inline cs2::CUIEngine::runScript PanoramaUiEnginePatterns::runScript() noexcept
{
    return GlobalContext::instance().panoramaPatternFinder("55 48 89 E5 41 57 41 56 49 89 D6 41 55 41 54 49 89 FC 53 48 81 EC"_pat).as<cs2::CUIEngine::runScript>();
}

inline cs2::CUIEngine::makeSymbol PanoramaUiEnginePatterns::makeSymbol() noexcept
{
    return GlobalContext::instance().panoramaPatternFinder("89 F7 48 89 D6 E9 ? ? ? ? 66 0F 1F 44 00 00 89 F7 48 89 D6"_pat).as<cs2::CUIEngine::makeSymbol>();
}

inline cs2::CUIEngine::onDeletePanel PanoramaUiEnginePatterns::onDeletePanel() noexcept
{
    return GlobalContext::instance().panoramaPatternFinder("55 48 89 E5 41 54 49 89 FC 53 48 89 F3 48 83 EC 10"_pat).as<cs2::CUIEngine::onDeletePanel>();
}
