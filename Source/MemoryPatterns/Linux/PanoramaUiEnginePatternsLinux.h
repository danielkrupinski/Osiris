#pragma once

#include <CS2/Panorama/CUIEngine.h>
#include <MemorySearch/BytePatternLiteral.h>

template <typename PatternFinders>
struct PanoramaUiEnginePatterns {
    const PatternFinders& patternFinders;

    [[nodiscard]] cs2::CUIEngine** uiEngine() const noexcept
    {
        return patternFinders.clientPatternFinder("48 89 E5 48 89 3D ? ? ? ? E8"_pat).add(6).abs().template as<cs2::CUIEngine**>();
    }

    [[nodiscard]] cs2::CUIEngine::getPanelHandle getPanelHandle() const noexcept
    {
        return patternFinders.panoramaPatternFinder("8B 97 ? ? ? ? 83 FA FF 74 2E"_pat).template as<cs2::CUIEngine::getPanelHandle>();
    }

    [[nodiscard]] cs2::CUIEngine::getPanelPointer getPanelPointer() const noexcept
    {
        return patternFinders.panoramaPatternFinder("8B 06 45 31 C0 85 C0"_pat).template as<cs2::CUIEngine::getPanelPointer>();
    }

    [[nodiscard]] cs2::CUIEngine::runScript runScript() const noexcept
    {
        return patternFinders.panoramaPatternFinder("55 48 89 E5 41 57 49 89 CF 41 56 49 89 F6 41 55 41"_pat).template as<cs2::CUIEngine::runScript>();
    }

    [[nodiscard]] cs2::CUIEngine::makeSymbol makeSymbol() const noexcept
    {
        return patternFinders.panoramaPatternFinder("89 F7 48 89 D6 E9 ? ? ? ? 66 0F 1F 44 00 00 89 F7 48 89 D6"_pat).template as<cs2::CUIEngine::makeSymbol>();
    }

    [[nodiscard]] cs2::CUIEngine::onDeletePanel onDeletePanel() const noexcept
    {
        return patternFinders.panoramaPatternFinder("55 48 89 E5 41 54 49 89 FC 53 48 89 F3 48 83 EC 10"_pat).template as<cs2::CUIEngine::onDeletePanel>();
    }
};
