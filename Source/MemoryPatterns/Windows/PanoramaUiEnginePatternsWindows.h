#pragma once

#include <MemoryPatterns/PanoramaUiEnginePatterns.h>
#include <MemorySearch/BytePatternLiteral.h>

inline cs2::CUIEngine::getPanelHandle PanoramaUiEnginePatterns::getPanelHandle() const noexcept
{
    return panoramaPatternFinder("8B 81 ? ? ? ? 4C 8B D9 83 F8 FF 74 41"_pat).as<cs2::CUIEngine::getPanelHandle>();
}

inline cs2::CUIEngine::getPanelPointer PanoramaUiEnginePatterns::getPanelPointer() const noexcept
{
    return panoramaPatternFinder("4C 63 02 4D 8B C8"_pat).as<cs2::CUIEngine::getPanelPointer>();
}

inline cs2::CUIEngine::runScript PanoramaUiEnginePatterns::runScript() const noexcept
{
    return panoramaPatternFinder("48 89 5C 24 ? 48 89 6C 24 ? 56 57 41 54 41 56 41 57 48 81 EC ? ? ? ? 4C 8B F1"_pat).as<cs2::CUIEngine::runScript>();
}

inline cs2::CUIEngine::makeSymbol PanoramaUiEnginePatterns::makeSymbol() const noexcept
{
    return panoramaPatternFinder("8B CA 49 8B D0"_pat).as<cs2::CUIEngine::makeSymbol>();
}

inline cs2::CUIEngine::onDeletePanel PanoramaUiEnginePatterns::onDeletePanel() const noexcept
{
    return panoramaPatternFinder("48 89 5C 24 ? 56 48 83 EC 20 8B 05"_pat).as<cs2::CUIEngine::onDeletePanel>();
}
