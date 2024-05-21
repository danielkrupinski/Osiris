#pragma once

#include <CS2/Classes/Panorama.h>
#include <MemorySearch/BytePatternLiteral.h>

template <typename PatternFinders>
struct PanoramaUiEnginePatterns {
    const PatternFinders& patternFinders;

    [[nodiscard]] cs2::CUIEngine** uiEngine() const noexcept
    {
        return patternFinders.clientPatternFinder("48 89 78 ? 48 89 0D ? ? ? ?"_pat).add(7).abs().template as<cs2::CUIEngine**>();
    }

    [[nodiscard]] cs2::CUIEngine::getPanelHandle getPanelHandle() const noexcept
    {
        return patternFinders.panoramaPatternFinder("8B 81 ? ? ? ? 4C 8B D9 83 F8 FF 74 41"_pat).template as<cs2::CUIEngine::getPanelHandle>();
    }

    [[nodiscard]] cs2::CUIEngine::getPanelPointer getPanelPointer() const noexcept
    {
        return patternFinders.panoramaPatternFinder("4C 63 02 4D 8B C8"_pat).template as<cs2::CUIEngine::getPanelPointer>();
    }

    [[nodiscard]] cs2::CUIEngine::runScript runScript() const noexcept
    {
        return patternFinders.panoramaPatternFinder("48 89 5C 24 ? 48 89 6C 24 ? 56 57 41 54 41 56 41 57 48 81 EC ? ? ? ? 4C 8B"_pat).template as<cs2::CUIEngine::runScript>();
    }

    [[nodiscard]] cs2::CUIEngine::makeSymbol makeSymbol() const noexcept
    {
        return patternFinders.panoramaPatternFinder("8B CA 49 8B D0"_pat).template as<cs2::CUIEngine::makeSymbol>();
    }

    [[nodiscard]] cs2::CUIEngine::onDeletePanel onDeletePanel() const noexcept
    {
        return patternFinders.panoramaPatternFinder("48 89 5C 24 ? 56 48 83 EC 20 8B 05"_pat).template as<cs2::CUIEngine::onDeletePanel>();
    }
};
