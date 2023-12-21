#pragma once

#include <CS2/Classes/Panorama.h>

struct PanelStylePatterns {
    [[nodiscard]] static cs2::CPanelStyle::SetProperty* setProperty() noexcept;
    [[nodiscard]] static cs2::CPanelStyle::StylePropertySymbols* stylePropertiesSymbols() noexcept;
};
