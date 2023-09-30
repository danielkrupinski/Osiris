#pragma once

#include <CS2/Classes/Panorama.h>

struct PanoramaUiEnginePatterns {
    [[nodiscard]] static cs2::CUIEngine::getPanelHandle getPanelHandle() noexcept;
    [[nodiscard]] static cs2::CUIEngine::getPanelPointer getPanelPointer() noexcept;
    [[nodiscard]] static cs2::CUIEngine::runScript runScript() noexcept;
    [[nodiscard]] static cs2::CUIEngine::makeSymbol makeSymbol() noexcept;
    [[nodiscard]] static cs2::CUIEngine::onDeletePanel onDeletePanel() noexcept;
};
