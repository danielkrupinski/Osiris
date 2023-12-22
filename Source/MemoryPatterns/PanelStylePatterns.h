#pragma once

#include <CS2/Classes/Panorama.h>

struct PanelStylePatterns {
    [[nodiscard]] cs2::CPanelStyle::SetProperty* setProperty() const noexcept;
    [[nodiscard]] cs2::CPanelStyle::StylePropertySymbols* stylePropertiesSymbols() const noexcept;
};
