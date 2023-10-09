#pragma once

#include <CS2/Classes/Panorama.h>
#include <MemoryPatterns/ClientPatterns.h>
#include <MemoryPatterns/PanoramaUiEnginePatterns.h>

struct PanoramaUiEngineImpl {
    [[nodiscard]] static const PanoramaUiEngineImpl& instance() noexcept;

    cs2::CUIEngine** thisptr{ ClientPatterns::uiEngine() };

    cs2::CUIEngine::getPanelHandle getPanelHandle{ PanoramaUiEnginePatterns::getPanelHandle() };
    cs2::CUIEngine::getPanelPointer getPanelPointer{ PanoramaUiEnginePatterns::getPanelPointer() };
    cs2::CUIEngine::runScript runScript{ PanoramaUiEnginePatterns::runScript() };
    cs2::CUIEngine::makeSymbol makeSymbol{ PanoramaUiEnginePatterns::makeSymbol() };
    cs2::CUIEngine::onDeletePanel onDeletePanel{ PanoramaUiEnginePatterns::onDeletePanel() };
};
