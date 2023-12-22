#pragma once

#include <CS2/Classes/Panorama.h>
#include <MemoryPatterns/ClientPatterns.h>
#include <MemoryPatterns/PanoramaUiEnginePatterns.h>

struct PanoramaUiEngineImpl {
    explicit PanoramaUiEngineImpl(const ClientPatterns& clientPatterns, const PanoramaUiEnginePatterns& panoramaUiEnginePatterns) noexcept
        : thisptr{clientPatterns.uiEngine()}
        , getPanelHandle{panoramaUiEnginePatterns.getPanelHandle()}
        , getPanelPointer{panoramaUiEnginePatterns.getPanelPointer()}
        , runScript{panoramaUiEnginePatterns.runScript()}
        , makeSymbol{panoramaUiEnginePatterns.makeSymbol()}
        , onDeletePanel{panoramaUiEnginePatterns.onDeletePanel()}
    {
    }

    [[nodiscard]] static const PanoramaUiEngineImpl& instance() noexcept;

    cs2::CUIEngine** thisptr;

    cs2::CUIEngine::getPanelHandle getPanelHandle;
    cs2::CUIEngine::getPanelPointer getPanelPointer;
    cs2::CUIEngine::runScript runScript;
    cs2::CUIEngine::makeSymbol makeSymbol;
    cs2::CUIEngine::onDeletePanel onDeletePanel;
};
