#pragma once

#include <CS2/Classes/Panorama.h>

struct PanoramaUiEnginePatterns {
    [[nodiscard]] cs2::CUIEngine::getPanelHandle getPanelHandle() const noexcept;
    [[nodiscard]] cs2::CUIEngine::getPanelPointer getPanelPointer() const noexcept;
    [[nodiscard]] cs2::CUIEngine::runScript runScript() const noexcept;
    [[nodiscard]] cs2::CUIEngine::makeSymbol makeSymbol() const noexcept;
    [[nodiscard]] cs2::CUIEngine::onDeletePanel onDeletePanel() const noexcept;
};
