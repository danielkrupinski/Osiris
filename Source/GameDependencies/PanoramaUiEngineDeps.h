#pragma once

#include <CS2/Classes/Panorama.h>

struct PanoramaUiEngineDeps {
    template <typename PanoramaUiEnginePatterns>
    explicit PanoramaUiEngineDeps(const PanoramaUiEnginePatterns& panoramaUiEnginePatterns) noexcept
        : thisptr{panoramaUiEnginePatterns.uiEngine()}
        , getPanelHandle{panoramaUiEnginePatterns.getPanelHandle()}
        , getPanelPointer{panoramaUiEnginePatterns.getPanelPointer()}
        , runScript{panoramaUiEnginePatterns.runScript()}
        , makeSymbol{panoramaUiEnginePatterns.makeSymbol()}
        , onDeletePanel{panoramaUiEnginePatterns.onDeletePanel()}
    {
    }

    [[nodiscard]] static const PanoramaUiEngineDeps& instance() noexcept;

    cs2::CUIEngine** thisptr;

    cs2::CUIEngine::getPanelHandle getPanelHandle;
    cs2::CUIEngine::getPanelPointer getPanelPointer;
    cs2::CUIEngine::runScript runScript;
    cs2::CUIEngine::makeSymbol makeSymbol;
    cs2::CUIEngine::onDeletePanel onDeletePanel;
};
